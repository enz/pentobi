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
#include "libboardgame_sgf/TreeIterator.h"
#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_sgf/Util.h"
#include "libboardgame_util/Exception.h"
#include "libboardgame_util/Log.h"
#include "libpentobi_base/Game.h"

namespace libpentobi_mcts {

using namespace std;
using boost::filesystem::exists;
using boost::filesystem::ofstream;
using boost::filesystem::rename;
using boost::format;
using libboardgame_base::PointTransfIdent;
using libboardgame_base::PointTransfRot270Refl;
using libboardgame_sgf::ChildIterator;
using libboardgame_sgf::MissingProperty;
using libboardgame_sgf::TreeIterator;
using libboardgame_sgf::TreeReader;
using libboardgame_sgf::util::write_tree;
using libboardgame_util::log;
using libboardgame_util::Exception;
using libpentobi_base::game_variant_classic;
using libpentobi_base::game_variant_classic_2;
using libpentobi_base::game_variant_duo;
using libpentobi_base::game_variant_junior;
using libpentobi_base::ColorMove;
using libpentobi_base::Game;
using libpentobi_base::Move;
using libpentobi_base::MoveInfo;
using libpentobi_base::MovePoints;

//-----------------------------------------------------------------------------

namespace {

int evaluation_level = 6;

} // namespace

//-----------------------------------------------------------------------------

BookBuilder::BookBuilder(GameVariant game_variant)
  : m_game_variant(game_variant),
    m_tree(game_variant),
    m_bd(new Board(game_variant)),
    m_updater(m_tree, *m_bd),
    m_player(new Player(game_variant, "")),
    m_moves(new ArrayList<Move, Move::range>)
{
    if (game_variant != game_variant_duo
        && game_variant != game_variant_junior
        && game_variant != game_variant_classic_2)
        throw Exception("BookBuilder supports only two-player game variants");
    m_expansion_parameter = 40; // TODO: Make configurable
}

void BookBuilder::add_leaf(const vector<ColorMove>& sequence, ColorMove mv)
{
    add_leaf(sequence, mv, PointTransfIdent<Point>());
    if (m_game_variant == game_variant_duo
        || m_game_variant == game_variant_junior)
        add_leaf(sequence, mv, PointTransfRot270Refl<Point>());
}

void BookBuilder::add_leaf(const vector<ColorMove>& sequence, ColorMove mv,
                           const PointTransform<Point>& transform)
{
    ColorMove transformed_mv = get_transformed(mv, transform);
    vector<ColorMove> transformed_sequence;
    BOOST_FOREACH(ColorMove mv, sequence)
        transformed_sequence.push_back(get_transformed(mv, transform));
    const Node* node = find_node(transformed_sequence);
    if (node == 0)
        return;
    if (m_tree.find_child_with_move(*node, transformed_mv) != 0)
        return;
    const Node& child = m_tree.create_new_child(*node);
    m_tree.set_move(child, transformed_mv);
}

void BookBuilder::add_value(const vector<ColorMove>& sequence, double value)
{
    add_value(sequence, value, PointTransfIdent<Point>());
    if (m_game_variant == game_variant_duo
        || m_game_variant == game_variant_junior)
        add_value(sequence, value, PointTransfRot270Refl<Point>());
}

void BookBuilder::add_value(const vector<ColorMove>& sequence, double value,
                            const PointTransform<Point>& transform)
{
    vector<ColorMove> transformed_sequence;
    BOOST_FOREACH(ColorMove mv, sequence)
        transformed_sequence.push_back(get_transformed(mv, transform));
    const Node* node = find_node(transformed_sequence);
    if (node == 0)
        return;
    m_tree.set_comment_property(*node, "v", value);
    m_tree.set_comment_property(*node, "h", value);
    if (node->has_parent() && are_children_evaluated(node->get_parent()))
    {
        while (true)
        {
            if (node->has_children())
            {
                update_value(*node);
                sort_children(*node);
            }
            if (node->has_parent())
            {
                const Node& parent = node->get_parent();
                double min_sibling_value = numeric_limits<double>::max();
                for (ChildIterator i(parent); i; ++i)
                {
                    double value = m_tree.get_comment_property<double>(*i, "v");
                    min_sibling_value = min(value, min_sibling_value);
                }
                for (ChildIterator i(parent); i; ++i)
                    update_priority(*i, min_sibling_value);
                node = &parent;
            }
            else
                break;
        }
    }
}

bool BookBuilder::are_children_evaluated(const Node& node) const
{
    for (ChildIterator i(node); i; ++i)
        if (! m_tree.has_comment_property(*i, "v"))
            return false;
    return true;
}

void BookBuilder::build(const path& file)
{
    if (exists(file))
    {
        log() << "Loading existing file '" << file << "'\n";
        TreeReader reader;
        reader.read(file);
        unique_ptr<Node> node = reader.get_tree_transfer_ownership();
        m_tree.init(node);
        if (m_tree.get_game_variant() != m_game_variant)
            throw Exception("File has wrong game variant");
    }
    else
        m_tree.init_game_variant(m_game_variant);
    while (true)
    {
        const Node* node;
        while ((node = find_unevaluated_node()) != 0)
        {
            m_updater.update(*node);
            log() << "Updating unevaluated node:\n" << *m_bd;
            double value = create_heuristic();
            log() << "Value: " << value << '\n';
            vector<ColorMove> sequence = get_sequence(*node);
            add_value(sequence, value);
            write(file);
        }
        node = &find_node_to_expand();
        m_updater.update(*node);
        log() << "Next node to expand:\n" << *m_bd;
        vector<ColorMove> moves = generate_moves();
        log() << "Moves: " << moves.size() << '\n';
        vector<ColorMove> sequence = get_sequence(*node);
        BOOST_FOREACH(ColorMove mv, moves)
            add_leaf(sequence, mv);
        write(file);
    }
}

double BookBuilder::create_heuristic()
{
    Search& search = m_player->get_search();
    // Do not avoid symmetric draw as second player, otherwise the book will not
    // see that moves are bad for the first player that allow the second player
    // to enforce a symmetric draw
    search.set_avoid_symmetric_draw(false);
    m_player->set_level(evaluation_level);
    m_player->set_use_book(false);
    m_player->genmove(*m_bd, m_bd->get_effective_to_play());
    return search.get_tree().get_root().get_value();
}

const Node& BookBuilder::find_node_to_expand() const
{
    const Node* node = &m_tree.get_root();
    while (node->has_children())
    {
        double lowest_priority = numeric_limits<double>::max();
        const Node* next_node = 0;
        for (ChildIterator i(*node); i; ++i)
        {
            double priority = m_tree.get_comment_property<double>(*i, "p");
            if (priority < lowest_priority)
            {
                lowest_priority = priority;
                next_node = &(*i);
            }
        }
        node = next_node;
    }
    return *node;
}

const Node* BookBuilder::find_node(const vector<ColorMove>& sequence) const
{
    const Node* node = &m_tree.get_root();
    BOOST_FOREACH(ColorMove mv, sequence)
    {
        node = m_tree.find_child_with_move(*node, mv);
        if (node == 0)
            break;
    }
    return node;
}

const Node* BookBuilder::find_unevaluated_node() const
{
    for (TreeIterator i(m_tree.get_root()); i; ++i)
        if (! m_tree.has_comment_property(*i, "h"))
            return &(*i);
    return 0;
}

vector<ColorMove> BookBuilder::generate_moves() const
{
    Color c = m_bd->get_effective_to_play();
    m_bd->gen_moves(c, *m_moves);
    unsigned int min_piece_size = 0;
    unsigned int nu_moves = m_bd->get_nu_moves();
    if (m_game_variant == game_variant_duo
        || m_game_variant == game_variant_junior)
    {
        if (nu_moves < 4)
            min_piece_size = 5;
        else if (nu_moves < 6)
            min_piece_size = 4;
    }
    else if (m_game_variant == game_variant_classic
             || m_game_variant == game_variant_classic_2)
    {
        if (nu_moves < 12)
            min_piece_size = 5;
        else if (nu_moves < 20)
            min_piece_size = 4;
    }
    vector<ColorMove> result;
    BOOST_FOREACH(Move mv, *m_moves)
    {
        const MoveInfo& info = m_bd->get_move_info(mv);
        if (m_bd->get_piece_info(info.piece).get_size() >= min_piece_size)
            result.push_back(ColorMove(c, mv));
    }
    return result;
}

vector<ColorMove> BookBuilder::get_sequence(const Node& node)
{
    vector<ColorMove> result;
    const Node* current = &node;
    while (current != &m_tree.get_root())
    {
        result.insert(result.begin(), m_tree.get_move(*current));
        current = &current->get_parent();
    }
    return result;
}

ColorMove BookBuilder::get_transformed(ColorMove mv,
                                   const PointTransform<Point>& transform) const
{
    if (mv.move.is_pass())
        return mv;
    unsigned int width = m_bd->get_geometry().get_width();
    unsigned int height = m_bd->get_geometry().get_height();
    MovePoints points;
    BOOST_FOREACH(Point p, m_bd->get_move_points(mv.move))
        points.push_back(transform.get_transformed(p, width, height));
    Move transformed_mv;
    m_bd->find_move(points, transformed_mv);
    return ColorMove(mv.color, transformed_mv);
}

double BookBuilder::inv_value(double value) const
{
    return 1 - value;
}

void BookBuilder::prune(const path& file, double prune_book_diff)
{
    if (! exists(file))
        throw Exception("Book file not found");
    TreeReader reader;
    reader.read(file);
    unique_ptr<Node> node = reader.get_tree_transfer_ownership();
    m_tree.init(node);
    if (m_tree.get_game_variant() != m_game_variant)
        throw Exception("File has wrong game variant");
    for (TreeIterator i(m_tree.get_root()); i; ++i)
    {
        // Delete all comment properties but v, the other properties are only
        // used during building the book. Reduce precision for v.
        double value = m_tree.get_comment_property<double>(*i, "v");
        m_tree.set_comment(*i, "");
        m_tree.set_comment_property<string>(*i, "v",
                                            str(format("%.3f") % value));
        if (! (*i).has_children())
            continue;
        if (! are_children_evaluated(*i))
        {
            const Node* child = (*i).get_first_child_or_null();
            while (child != 0)
            {
                const Node* sibling = child->get_sibling();
                m_tree.truncate(*child);
                child = sibling;
            }
            continue;
        }
        double best_value = -numeric_limits<double>::max();
        for (ChildIterator j(*i); j; ++j)
        {
            double value =
                inv_value(m_tree.get_comment_property<double>(*j, "v"));
            best_value = max(value, best_value);
        }
        const Node* child = (*i).get_first_child_or_null();
        while (child != 0)
        {
            double value =
                inv_value(m_tree.get_comment_property<double>(*child, "v"));
            double value_diff = best_value - value;
            LIBBOARDGAME_ASSERT(value_diff >= 0);
            const Node* sibling = child->get_sibling();
            if (value_diff > prune_book_diff)
                m_tree.truncate(*child);
            child = sibling;
        }
    }
    write(file);
}

void BookBuilder::sort_children(const Node& node)
{
    unsigned int nu_children = node.get_nu_children();
    for (unsigned int i = 0; i < nu_children; ++i)
    {
        double best_value = -numeric_limits<double>::max();
        unsigned int best_child = 0; // Init to avoid compiler warning
        for (unsigned int j = i; j < nu_children; ++j)
        {
            const Node& child = node.get_child(j);
            double value = m_tree.get_comment_property<double>(child, "v");
            if (value > best_value)
            {
                best_value = value;
                best_child = j;
            }
        }
        m_tree.make_first_child(node.get_child(best_child));
    }
}

void BookBuilder::update_priority(const Node& node, double min_sibling_value)
{
    if (&node == &m_tree.get_root())
        log() << "Updating priority, root node\n";
    else
        log() << "Updating priority, node with move: "
              << m_bd->to_string(m_tree.get_move(node).move) << '\n';
    if (m_tree.has_comment_property(node, "p"))
        log() << "Old priority: "
              << m_tree.get_comment_property<double>(node, "p") << '\n';
    double value = m_tree.get_comment_property<double>(node, "v");
    double value_diff = value - min_sibling_value;
    double min_child_priority = 0;
    if (node.has_children() && are_children_evaluated(node))
    {
        min_child_priority = numeric_limits<double>::max();
        for (ChildIterator i(node); i; ++i)
        {
            double child_priority =
                m_tree.get_comment_property<double>(*i, "p");
            min_child_priority = min(child_priority, min_child_priority);
        }
    }
    double priority =
        1 + m_expansion_parameter * value_diff + min_child_priority;
    log() << "New priority: " << priority << " (min_sibling_value="
          << min_sibling_value << ", value_diff=" << value_diff;
    if (node.has_children())
        log() << ", min_child_priority=" << min_child_priority;
    log() << ")\n";
    m_tree.set_comment_property<double>(node, "p", priority);
}

void BookBuilder::update_value(const Node& node)
{
    LIBBOARDGAME_ASSERT(node.has_children());
    if (&node == &m_tree.get_root())
        log() << "Updating value, root node\n";
    else
        log() << "Updating value, node with move: "
              << m_bd->to_string(m_tree.get_move(node).move) << '\n';
    log() << "Old value: " << m_tree.get_comment_property<double>(node, "v")
          << '\n';
    double value = numeric_limits<double>::max();
    for (ChildIterator i(node); i; ++i)
    {
        double child_value = m_tree.get_comment_property<double>(*i, "v");
        value = min(child_value, value);
    }
    value = inv_value(value);
    log() << "New value: " << value << '\n';
    m_tree.set_comment_property<double>(node, "v", value);
}

void BookBuilder::write(const path& file) const
{
    path tmp_file = file.string() + ".new";
    {
        ofstream out(tmp_file);
        write_tree(out, m_tree.get_root());
        if (! out)
            throw Exception(format("Could not write '%1%'") % file);
    }
    if (exists(file))
        remove(file);
    rename(tmp_file, file);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
