//-----------------------------------------------------------------------------
/** @file pentobi_gtp/Engine.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Engine.h"

#include <fstream>
#include <boost/format.hpp>
#include "libboardgame_sgf/TreeReader.h"
#include "libpentobi_base/Tree.h"
#include "libpentobi_mcts/Util.h"

namespace pentobi_gtp {

using boost::format;
using libboardgame_gtp::Failure;
using libboardgame_mcts::ChildIterator;
using libboardgame_sgf::TreeReader;
using libboardgame_util::log;
using libpentobi_base::Board;
using libpentobi_base::PieceInfo;
using libpentobi_base::Tree;
using libpentobi_mcts::Move;
using libpentobi_mcts::State;
using libpentobi_mcts::ValueType;

//-----------------------------------------------------------------------------

namespace {

bool is_child_better(const Search::Node* n1, const Search::Node* n2)
{
    ValueType visit_count1 = n1->get_visit_count();
    ValueType visit_count2 = n2->get_visit_count();
    if (visit_count1 != visit_count2)
        return visit_count1 > visit_count2;
    if (n1->get_count() > 0 && n2->get_count() > 0)
        return n1->get_value() > n2->get_value();
    return false;
}

} // namespace

//-----------------------------------------------------------------------------

Engine::Engine(GameVariant game_variant, int level, bool use_book,
               const path& books_dir, size_t memory)
    : libpentobi_base::Engine(game_variant)
{
    create_player(game_variant, books_dir, memory);
    get_mcts_player().set_use_book(use_book);
    get_mcts_player().set_level(level);
    add("gen_playout_move", &Engine::cmd_gen_playout_move);
    add("get_value", &Engine::cmd_get_value);
    add("param", &Engine::cmd_param);
    add("move_values", &Engine::cmd_move_values);
    add("save_tree", &Engine::cmd_save_tree);
}

Engine::~Engine() throw()
{
}

void Engine::cmd_get_value(Response& response)
{
    const Search::Node& root = get_search().get_tree().get_root();
    if (root.get_count() == 0)
        throw Failure("root node has no count");
    response << root.get_value();
}

void Engine::cmd_move_values(Response& response)
{
    const Search& search = get_search();
    const Board& bd = get_board();
    vector<const Search::Node*> children;
    for (ChildIterator<Move> i(search.get_tree().get_root()); i; ++i)
        children.push_back(&(*i));
    sort(children.begin(), children.end(), libpentobi_mcts::util::compare_node);
    response << fixed;
    BOOST_FOREACH(const Search::Node* node, children)
    {
        response << setprecision(0) << node->get_visit_count() << ' ';
        ValueType count = node->get_count();
        response << setprecision(0) << count << ' ';
        if (count > 0)
            response << setprecision(3) << node->get_value();
        else
            response << '-';
        ValueType rave_count = node->get_rave_count();
        response << ' ' << setprecision(0) << rave_count << ' ';
        if (rave_count > 0)
            response << setprecision(3) << node->get_rave_value();
        else
            response << '-';
        response << ' ' << bd.to_string(node->get_move()) << '\n';
    }
}

void Engine::cmd_save_tree(const Arguments& args)
{
    const Search& search = get_search();
    if (! search.get_last_state().is_valid())
        throw Failure("no search tree");
    string file = args.get();
    ofstream out(file.c_str());
    libpentobi_mcts::util::dump_tree(out, search);
}

void Engine::cmd_param(const Arguments& args, Response& response)
{
    Player& p = get_mcts_player();
    Search& s = get_search();
    if (args.get_size() == 0)
        response
            << "avoid_symmetric_draw " << s.get_avoid_symmetric_draw() << '\n'
            << "auto_param " << s.get_auto_param() << '\n'
            << "bias_term_constant " << s.get_bias_term_constant() << '\n'
            << "detect_symmetry " << s.get_detect_symmetry() << '\n'
            << "expand_threshold " << s.get_expand_threshold() << '\n'
            << "fixed_simulations " << p.get_fixed_simulations() << '\n'
            << "last_good_reply " << s.get_last_good_reply() << '\n'
            << "level " << p.get_level() << '\n'
            << "rave " << s.get_rave() << '\n'
            << "reuse_subtree " << s.get_reuse_subtree() << '\n'
            << "score_modification " << s.get_score_modification() << '\n'
            << "use_book " << p.get_use_book() << '\n'
            << "widening_parameter " << s.get_widening_parameter() << '\n';
    else
    {
        args.check_size(2);
        string name = args.get(0);
        if (name == "avoid_symmetric_draw")
            s.set_avoid_symmetric_draw(args.get<bool>(1));
        else if (name == "auto_param")
            s.set_auto_param(args.get<bool>(1));
        else if (name == "bias_term_constant")
            s.set_bias_term_constant(args.get<ValueType>(1));
        else if (name == "detect_symmetry")
            s.set_detect_symmetry(args.get<bool>(1));
        else if (name == "expand_threshold")
            s.set_expand_threshold(args.get<ValueType>(1));
        else if (name == "fixed_simulations")
            p.set_fixed_simulations(args.get<ValueType>(1));
        else if (name == "last_good_reply")
            s.set_last_good_reply(args.get<bool>(1));
        else if (name == "level")
            p.set_level(args.get<int>(1));
        else if (name == "rave")
            s.set_rave(args.get<bool>(1));
        else if (name == "reuse_subtree")
            s.set_reuse_subtree(args.get<bool>(1));
        else if (name == "score_modification")
            s.set_score_modification(args.get<ValueType>(1));
        else if (name == "use_book")
            p.set_use_book(args.get<bool>(1));
        else if (name == "widening_parameter")
            s.set_widening_parameter(args.get<ValueType>(1));
        else
            throw Failure(format("unknown parameter '%1%'") % name);
    }
}

void Engine::create_player(GameVariant game_variant, const path& books_dir,
                           size_t memory)
{
    m_player.reset(new Player(game_variant, books_dir, memory));
    set_player(*m_player);
}

void Engine::cmd_gen_playout_move(Response& response)
{
    State& state = get_mcts_player().get_search().get_state();
    state.start_search();
    state.start_simulation(0);
    state.start_playout();
    if (! state.gen_and_play_playout_move(Move::null()))
        throw Failure("terminal playout position");
    const Board& bd = get_board();
    response << bd.to_string(state.get_move(state.get_nu_moves() - 1).move);
}

Player& Engine::get_mcts_player()
{
    try
    {
        return dynamic_cast<Player&>(*m_player);
    }
    catch (const bad_cast&)
    {
        throw Failure("current player is not mcts player");
    }
}

Search& Engine::get_search()
{
    return get_mcts_player().get_search();
}

void Engine::set_deterministic()
{
    try
    {
        get_search().set_deterministic();
    }
    catch (Failure&)
    {
        // Ignore if player is not MCTS player
    }
}

//-----------------------------------------------------------------------------

} // namespace pentobi_gtp
