//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/BookBuilder.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BookBuilder.h"

#include <fstream>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include "libboardgame_sgf/MissingProperty.h"
#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_sgf/Util.h"
#include "libboardgame_util/Exception.h"
#include "libboardgame_util/Log.h"
#include "libpentobi_base/Game.h"
#include "libpentobi_mcts/Player.h"

namespace libpentobi_mcts {

using namespace std;
using boost::filesystem::exists;
using boost::filesystem::ofstream;
using boost::format;
using libboardgame_base::Transform;
using libboardgame_sgf::ChildIterator;
using libboardgame_sgf::Node;
using libboardgame_sgf::MissingProperty;
using libboardgame_sgf::TreeReader;
using libboardgame_sgf::util::write_tree;
using libboardgame_util::log;
using libboardgame_util::Exception;
using libpentobi_base::game_variant_classic_2;
using libpentobi_base::game_variant_duo;
using libpentobi_base::Board;
using libpentobi_base::ColorMove;
using libpentobi_base::Game;
using libpentobi_base::Move;
using libpentobi_base::MovePoints;
using libpentobi_base::Tree;
using libpentobi_mcts::Player;

//-----------------------------------------------------------------------------

namespace {

int exploration_move_level = 3;

int evaluation_level = 6;

RandomGenerator random;

void sort_children(Game& game, const Node& node)
{
    const Tree& tree = game.get_tree();
    unsigned int nu_children = node.get_nu_children();
    for (unsigned int i = 0; i < nu_children; ++i)
    {
        double min_value = numeric_limits<double>::max();
        unsigned int best_child = 0; // Init to avoid compiler warning
        for (unsigned int j = i; j < nu_children; ++j)
        {
            const Node& child = node.get_child(j);
            double value = Book::get_value(tree, child);
            if (value < min_value)
            {
                min_value = value;
                best_child = j;
            }
        }
        game.goto_node(node.get_child(best_child));
        game.make_first_child();
    }
}

void add_value(Game& game, const Node& node, double value)
{
    game.goto_node(node);
    unsigned int count = 0; // Init to avoid compiler warning
    double old_value = 0; // Init to avoid compiler warning
    try
    {
        Book::get_value_and_count(game.get_tree(), game.get_current(),
                                  old_value, count);
    }
    catch (const Exception&)
    {
        log() << "Add value " << value << " 1\n";
        Book::set_value_and_count(game, value, 1);
        if (node.has_parent())
            sort_children(game, node.get_parent());
        return;
    }
    double new_value = (count * old_value + value) / (count + 1);
    unsigned int new_count = count + 1;
    log() << "Add value " << new_value << ' ' << new_count << '\n';
    Book::set_value_and_count(game, new_value, new_count);
    if (node.has_parent())
        sort_children(game, node.get_parent());
}

ColorMove get_transformed(const Game& game, ColorMove mv, Transform transform)
{
    if (mv.move.is_pass())
        return mv;
    const Board& bd = game.get_board();
    MovePoints points;
    BOOST_FOREACH(Point p, bd.get_move_points(mv.move))
        points.push_back(transform.get_transformed(p, bd.get_size()));
    Move transformed_mv;
    bd.find_move(points, transformed_mv);
    return ColorMove(mv.color, transformed_mv);
}

void add_value(Game& game, const vector<ColorMove>& moves, Transform transform,
               double value)
{
    vector<ColorMove> transformed_moves;
    BOOST_FOREACH(ColorMove mv, moves)
        transformed_moves.push_back(get_transformed(game, mv, transform));
    const Tree& tree = game.get_tree();
    const Node* node = &tree.get_root();
    BOOST_FOREACH(ColorMove mv, transformed_moves)
    {
        const Node* child = tree.find_child_with_move(*node, mv);
        if (child == 0)
        {
            game.goto_node(*node);
            game.play(mv, true);
            child = &game.get_current();
        }
        node = child;
    }
    add_value(game, *node, value);
}

void add_value(Game& game, const vector<ColorMove>& moves, double value)
{
    add_value(game, moves, Transform(Transform::identity), value);
    if (game.get_game_variant() == game_variant_duo)
    {
        Transform transform_mirror(Transform::rotate_270_mirror);
        if (moves.size() == 0)
            return;
        if (moves[0] != get_transformed(game, moves[0], transform_mirror))
            add_value(game, moves, transform_mirror, value);
    }
}

bool play_exploration_move(Game& game, Player& player, const Node* current,
                               const vector<ColorMove>& moves)
{
    log() << "Playing exploration move\n";
    game.goto_node(*current);
    const Tree& tree = game.get_tree();
    Color to_play = game.get_to_play();
    // Don't let the player exploit symmetric draw when generating move
    // candidates, otherwise it will probably always go for a symmetric draw
    // with White if it can, and we also want explore some other moves if
    // the user of the book wants to avoid symmetric draws as White.
    player.get_search().set_detect_symmetry(false);
    player.set_level(exploration_move_level);
    Move mv = player.genmove(to_play);
    if (mv.is_null())
    {
        log() << "Move candidate creation failed. Terminal?\n";
        return false;
    }
    ColorMove color_mv(to_play, mv);
    if (tree.find_child_with_move(*current, color_mv) != 0)
    {
        log() << "Move candidate already existed\n";
        return false;
    }
    game.play(color_mv, false);
    log() << "Exploration move:\n" << game.get_board();
    return true;
}

bool play_sequence(Game& game, Player& player, vector<ColorMove>& moves,
                   double& value)
{
    const Board& bd = game.get_board();
    const Tree& tree = game.get_tree();
    const Search& search = player.get_search();
    moves.clear();
    const Node* current = &game.get_root();
    while (true)
    {
        game.goto_node(*current);
        log() << "Current position:\n" << bd;
        if (! current->has_children())
        {
            log() << "Current node has no children. Generating move.\n";
            Color to_play = game.get_to_play();
            player.get_search().set_detect_symmetry(true);
            player.get_search().set_avoid_symmetric_draw(false);
            player.set_level(evaluation_level);
            Move mv = player.genmove(to_play);
            value = search.get_tree().get_root().get_value();
            log() << "Value: " << value << '\n';
            if (mv.is_null())
                return false;
            ColorMove color_mv(to_play, mv);
            game.play(color_mv, false);
            log() << "Move:\n" << bd;
            moves.push_back(color_mv);
            return true;
        }
        game.goto_node(*current);
        bool played_exploration_move = false;
        if (random.generate_float() < 0.15)
            played_exploration_move =
                play_exploration_move(game, player, current, moves);
        if (played_exploration_move)
            current = &game.get_current();
        else
            current = Book::select_child(random, bd, game.get_to_play(), tree,
                                         *current, 0.03, 1);
        moves.push_back(tree.get_move(*current));
    }
}

void prune(Tree& tree, const Node& node, unsigned int min_count, bool is_root)
{
    if (! is_root)
    {
        double value;
        unsigned int count;
        Book::get_value_and_count(tree, node, value, count);
        if (count < min_count)
        {
            tree.truncate(node);
            return;
        }
    }
    // ChildIterator cannot be used directly if children are potentially removed
    vector<const Node*> children;
    for (ChildIterator i(node); i; ++i)
        children.push_back(&(*i));
    BOOST_FOREACH(const Node* child, children)
        prune(tree, *child, min_count, false);
}

} // namespace

//-----------------------------------------------------------------------------

void build_book(const path& file, GameVariant game_variant)
{
    if (game_variant != game_variant_duo
        && game_variant != game_variant_classic_2)
        throw Exception("BookBuilder supports only two-player game variants");
    Game game(game_variant);
    if (exists(file))
    {
        log() << "Loading existing file '" << file << "'\n";
        TreeReader reader;
        reader.read(file);
        unique_ptr<Node> tree = reader.get_tree_transfer_ownership();
        game.init(tree);
        if (game.get_game_variant() != game_variant)
            throw Exception("File has incompatible game variant");
    }
    Player player(game.get_board(), game_variant);
    player.set_use_book(false);
    for (unsigned int i = 0; ; ++i)
    {
        log() << "==========================================================\n"
              << "Sequence " << i << "\n"
              << "==========================================================\n";
        vector<ColorMove> moves;
        double value;
        if (! play_sequence(game, player, moves, value))
        {
            log() << "Player generated null move. Terminal position?\n";
            continue;
        }
        log() << "Update values with value "
              << (moves.size() % 2 != 0 ? value : 1 - value)
              << '\n';
        vector<ColorMove> sub_sequence;
        for (unsigned int i = 0; i < moves.size(); ++i)
        {
            sub_sequence.push_back(moves[i]);
            double node_value;
            if (sub_sequence.size() % 2 == moves.size() % 2)
                node_value = value;
            else
                node_value = 1 - value;
            add_value(game, sub_sequence, node_value);
        }
        ofstream out(file);
        write_tree(out, game.get_root());
    }
}

void prune_book(const path& file, unsigned int min_count)
{
    TreeReader reader;
    reader.read(file);
    unique_ptr<Node> root = reader.get_tree_transfer_ownership();
    Tree tree(root);
    prune(tree, tree.get_root(), min_count, true);
    ofstream out(file);
    write_tree(out, tree.get_root());
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
