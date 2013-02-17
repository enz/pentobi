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
using libboardgame_util::ArrayList;
using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::Grid;
using libpentobi_base::Move;
using libpentobi_base::MoveList;
using libpentobi_base::MoveMarker;
using libpentobi_base::PieceInfo;
using libpentobi_base::Point;
using libpentobi_base::Tree;
using libpentobi_mcts::Float;
using libpentobi_mcts::Move;
using libpentobi_mcts::State;

//-----------------------------------------------------------------------------

Engine::Engine(Variant variant, int level, bool use_book,
               const path& books_dir, unsigned nu_threads, size_t memory)
    : libpentobi_base::Engine(variant)
{
    create_player(variant, books_dir, nu_threads, memory);
    get_mcts_player().set_use_book(use_book);
    get_mcts_player().set_level(level);
    add("gen_playout_move", &Engine::cmd_gen_playout_move);
    add("get_value", &Engine::cmd_get_value);
    add("name", &Engine::cmd_name);
    add("param", &Engine::cmd_param);
    add("move_values", &Engine::cmd_move_values);
    add("moves_stat", &Engine::cmd_moves_stat);
    add("save_tree", &Engine::cmd_save_tree);
    add("version", &Engine::cmd_version);
}

Engine::~Engine() throw()
{
}

void Engine::cmd_get_value(Response& response)
{
    response << get_search().get_tree().get_root().get_value();
}

void Engine::cmd_move_values(Response& response)
{
    auto& search = get_search();
    auto& tree = search.get_tree();
    auto& bd = get_board();
    vector<const Search::Node*> children;
    for (Search::ChildIterator i(tree, tree.get_root()); i; ++i)
        children.push_back(&(*i));
    sort(children.begin(), children.end(), libpentobi_mcts::util::compare_node);
    response << fixed;
    for (const auto node : children)
        response << setprecision(0) << node->get_visit_count() << ' '
                 << setprecision(1) << node->get_value_count() << ' '
                 << setprecision(3) << node->get_value() << ' '
                 << bd.to_string(node->get_move()) << '\n';
}

void Engine::cmd_moves_stat(const Arguments& args, Response& response)
{
    Color c = get_color_arg(args);
    auto& bd = get_board();
    Grid<unsigned> nu_moves_grid(bd.get_geometry(), 0);
    MoveList moves;
    MoveMarker marker;
    for (Point p : bd.get_attach_points(c))
    {
        bd.gen_moves(c, p, marker, moves);
        for (Move mv : moves)
            for (Point p : bd.get_move_info(mv))
                ++nu_moves_grid[p];
        marker.clear_all_set_known(moves);
        moves.clear();
    }
    response << '\n' << nu_moves_grid;
}

void Engine::cmd_name(Response& response)
{
    response.set("Pentobi");
}

void Engine::cmd_save_tree(const Arguments& args)
{
    auto& search = get_search();
    if (! search.get_last_state().is_valid())
        throw Failure("no search tree");
    ofstream out(args.get());
    libpentobi_mcts::util::dump_tree(out, search);
}

void Engine::cmd_param(const Arguments& args, Response& response)
{
    auto& p = get_mcts_player();
    auto& s = get_search();
    if (args.get_size() == 0)
        response
            << "avoid_symmetric_draw " << s.get_avoid_symmetric_draw() << '\n'
            << "auto_param " << s.get_auto_param() << '\n'
            << "bias_term_constant " << s.get_bias_term_constant() << '\n'
            << "detect_symmetry " << s.get_detect_symmetry() << '\n'
            << "expand_threshold " << s.get_expand_threshold() << '\n'
            << "fixed_simulations " << p.get_fixed_simulations() << '\n'
            << "level " << p.get_level() << '\n'
            << "rave_max_child_count " << s.get_rave_max_child_count() << '\n'
            << "rave_max_parent_count " << s.get_rave_max_parent_count() << '\n'
            << "rave_weight " << s.get_rave_weight() << '\n'
            << "reuse_subtree " << s.get_reuse_subtree() << '\n'
            << "score_modification " << s.get_score_modification() << '\n'
            << "use_book " << p.get_use_book() << '\n';
    else
    {
        args.check_size(2);
        string name = args.get(0);
        if (name == "avoid_symmetric_draw")
            s.set_avoid_symmetric_draw(args.get<bool>(1));
        else if (name == "auto_param")
            s.set_auto_param(args.get<bool>(1));
        else if (name == "bias_term_constant")
            s.set_bias_term_constant(args.get<Float>(1));
        else if (name == "detect_symmetry")
            s.set_detect_symmetry(args.get<bool>(1));
        else if (name == "expand_threshold")
            s.set_expand_threshold(args.get<Float>(1));
        else if (name == "fixed_simulations")
            p.set_fixed_simulations(args.get<Float>(1));
        else if (name == "level")
            p.set_level(args.get<int>(1));
        else if (name == "rave_max_child_count")
            s.set_rave_max_child_count(args.get<Float>(1));
        else if (name == "rave_max_parent_count")
            s.set_rave_max_parent_count(args.get<Float>(1));
        else if (name == "rave_weight")
            s.set_rave_weight(args.get<Float>(1));
        else if (name == "reuse_subtree")
            s.set_reuse_subtree(args.get<bool>(1));
        else if (name == "score_modification")
            s.set_score_modification(args.get<Float>(1));
        else if (name == "use_book")
            p.set_use_book(args.get<bool>(1));
        else
            throw Failure(format("unknown parameter '%1%'") % name);
    }
}

void Engine::cmd_version(Response& response)
{
    string version;
#ifdef VERSION
    version = VERSION;
#endif
    if (version.empty())
        version = "UNKNOWN";
    // By convention, the version string of unreleased versions contains the
    // string UNKNOWN (appended to the last released version). In this case, or
    // if VERSION was undefined, we append the build date.
    if (version.find("UNKNOWN") != string::npos)
    {
        version.append(" (");
        version.append(__DATE__);
        version.append(")");
    }
#if LIBBOARDGAME_DEBUG
    version.append(" (dbg)");
#endif
    response.set(version);
}

void Engine::create_player(Variant variant, const path& books_dir,
                           unsigned nu_threads, size_t memory)
{
    m_player.reset(new Player(variant, books_dir, nu_threads, memory));
    set_player(*m_player);
}

void Engine::cmd_gen_playout_move(Response& response)
{
    auto& state = get_mcts_player().get_search().get_state(0);
    state.start_search();
    state.start_simulation(0);
    state.finish_in_tree();
    if (! state.gen_and_play_playout_move(Move::null(), Move::null()))
        throw Failure("terminal playout position");
    auto& bd = get_board();
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

void Engine::use_cpu_time(bool enable)
{
    get_mcts_player().use_cpu_time(enable);
}

//-----------------------------------------------------------------------------

} // namespace pentobi_gtp
