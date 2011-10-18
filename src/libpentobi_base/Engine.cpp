//-----------------------------------------------------------------------------
/** @file libpentobi_base/Engine.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Engine.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_sgf/Util.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_util/RandomGenerator.h"

namespace libpentobi_base {

using boost::algorithm::to_lower;
using boost::filesystem::ifstream;
using boost::filesystem::path;
using boost::format;
using libboardgame_gtp::Failure;
using libboardgame_sgf::InvalidPropertyValue;
using libboardgame_sgf::TreeReader;
using libboardgame_sgf::util::get_last_node;
using libboardgame_util::log;
using libboardgame_util::ArrayList;
using libboardgame_util::RandomGenerator;
using libpentobi_base::BoardConst;
using libpentobi_base::Move;
using libpentobi_base::Point;
using libpentobi_base::Color;

//-----------------------------------------------------------------------------

Engine::Engine(GameVariant game_variant)
    : m_accept_illegal(false),
      m_show_board(false),
      m_game(game_variant),
      m_player(0)
{
    add("all_legal", &Engine::cmd_all_legal);
    add("clear_board", &Engine::cmd_clear_board);
    add("final_score", &Engine::cmd_final_score);
    add("loadsgf", &Engine::cmd_loadsgf);
    add("point_integers", &Engine::cmd_point_integers);
    add("p", &Engine::cmd_p);
    add("play", &Engine::cmd_play);
    add("set_game", &Engine::cmd_set_game);
    add("showboard", &Engine::cmd_showboard);
    add("undo", &Engine::cmd_undo);
}

Engine::~Engine() throw()
{
}

void Engine::board_changed()
{
    if (m_show_board)
        log() << get_board();
}

void Engine::cmd_all_legal(const Arguments& args, Response& response)
{
    const Board& bd = get_board();
    unique_ptr<ArrayList<Move, Move::range>>
        moves(new ArrayList<Move, Move::range>());
    bd.gen_moves(get_color_arg(args), *moves);
    BOOST_FOREACH(Move mv, *moves)
        response << bd.to_string(mv) << '\n';
}

void Engine::cmd_clear_board()
{
    m_game.init();
    board_changed();
}

void Engine::cmd_final_score(Response& response)
{
    const Board& bd = get_board();
    GameVariant game_variant = bd.get_game_variant();
    if (game_variant == game_variant_classic)
    {
        for (ColorIterator i(bd.get_nu_colors()); i; ++i)
            response << bd.get_points_with_bonus(*i) << ' ';
    }
    else
    {
        double game_result;
        int score = bd.get_score(Color(0), game_result);
        if (game_result == 1)
            response << format("B+%1%") % score;
        else if (game_result == 0)
            response << format("W+%1%") % -score;
        else
            response << "0";
    }
}

void Engine::cmd_g(Response& response)
{
    genmove(get_board().get_to_play(), response);
}

void Engine::cmd_genmove(const Arguments& args, Response& response)
{
    genmove(get_color_arg(args), response);
}

void Engine::cmd_loadsgf(const Arguments& args)
{
    args.check_size_less_equal(2);
    path file = args.get<path>(0);
    int move_number = -1;
    if (args.get_size() == 2)
        move_number = args.get_min<int>(1, 1);
    try
    {
        TreeReader reader;
        reader.read(file);
        unique_ptr<Node> tree = reader.get_tree_transfer_ownership();
        m_game.init(tree);
        const Node* node = 0;
        if (move_number != -1)
            node = m_game.get_tree().get_node_with_move_number(move_number - 1);
        if (node == 0)
            node = &get_last_node(m_game.get_root());
        m_game.goto_node(*node);
        board_changed();
    }
    catch (const Exception& e)
    {
        throw Failure(e.what());
    }
}

void Engine::cmd_p(const Arguments& args)
{
    play(get_board().get_to_play(), args, 0);
}

void Engine::cmd_play(const Arguments& args)
{
    play(get_color_arg(args, 0), args, 1);
}

void Engine::cmd_point_integers(Response& response)
{
    const Geometry* geometry =
        Geometry::get(Point::max_width, Point::max_height);
    Grid<int> grid(*geometry);
    for (GeometryIterator i(*geometry); i; ++i)
        grid[*i] = (*i).to_int();
    response << '\n' << grid;
}

void Engine::cmd_reg_genmove(const Arguments& args, Response& response)
{
    RandomGenerator::set_global_seed_last();
    Move move = get_player().genmove(get_color_arg(args));
    if (move.is_null())
        throw Failure("player failed to generate a move");
    response << get_board().to_string(move, true);
}

/** Set the game variant.
    Arguments: Blokus|Blokus Two-Player|Blokus Duo<br>
    This command is similar to the command that is used by Quarry
    (http://home.gna.org/quarry/) to set a game at GTP engines that could
    support multiple games. */
void Engine::cmd_set_game(const Arguments& args)
{
    string arg = args.get_line();
    to_lower(arg);
    if (arg == "blokus")
        m_game.init(game_variant_classic);
    else if (arg == "blokus two-player")
        m_game.init(game_variant_classic_2);
    else if (arg == "blokus duo")
        m_game.init(game_variant_duo);
    else
        throw Failure("invalid argument");
    board_changed();
}

void Engine::cmd_showboard(Response& response)
{
    response << '\n' << get_board();
}

void Engine::cmd_undo()
{
    const Board& bd = get_board();
    if (bd.get_nu_moves() == 0)
        throw Failure("cannot undo");
    m_game.undo();
    board_changed();
}

void Engine::genmove(Color c, Response& response)
{
    Move mv = get_player().genmove(c);
    if (mv.is_null())
        throw Failure("player failed to generate a move");
    const Board& bd = get_board();
    if (! bd.is_legal(c, mv))
        throw Failure(format("player generated illegal move: %1%")
                      % bd.to_string(mv));
    m_game.play(c, mv, true);
    response << bd.to_string(mv, true);
    board_changed();
}

Color Engine::get_color_arg(const Arguments& args) const
{
    if (args.get_size() > 1)
        throw Failure("too many arguments");
    return get_color_arg(args, 0);
}

Color Engine::get_color_arg(const Arguments& args, unsigned int i) const
{
    string s = args.get_tolower(i);
    const Board& bd = get_board();
    GameVariant variant = bd.get_game_variant();
    if (variant == game_variant_classic || variant == game_variant_classic_2)
    {
        if (s == "1" || s == "blue")
            return Color(0);
        if (s == "2" || s == "yellow")
            return Color(1);
        if (s == "3" || s == "red")
            return Color(2);
        if (s == "4" || s == "green")
            return Color(3);
    }
    if (variant == game_variant_duo)
    {
        if (s == "blue" || s == "black" || s == "b")
            return Color(0);
        if (s == "green" || s == "white" || s == "w")
            return Color(1);
    }
    throw Failure(format("invalid color argument '%1%'") % s);
}

Player& Engine::get_player() const
{
    if (m_player == 0)
        throw Failure("no player set");
    return *m_player;
}

void Engine::play(Color c, const Arguments& args, unsigned int arg_move_begin)
{
    const Board& bd = get_board();
    if (bd.get_nu_moves() >= Board::max_game_moves)
        throw Failure("too many moves");
    Move mv;
    if (args.get_size() == arg_move_begin + 1
        && args.get_tolower(arg_move_begin) == "pass")
        mv = Move::pass();
    else
    {
        MovePoints points;
        for (unsigned int i = arg_move_begin; i < args.get_size(); ++i)
        {
            if (points.size() >= MovePoints::max_size)
                throw Failure("invalid move (too many points)");
            points.push_back(args.get<Point>(i));
        }
        if (points.empty())
            throw Failure("invalid move (empty point list)");
        if (! bd.find_move(points, mv))
            throw Failure("invalid move");
    }
    if (! m_accept_illegal && ! bd.is_legal(c, mv))
        throw Failure("illegal move");
    m_game.play(c, mv, true);
    board_changed();
}

void Engine::set_player(Player& player)
{
    m_player = &player;
    add("genmove", &Engine::cmd_genmove);
    add("g", &Engine::cmd_g);
    add("reg_genmove", &Engine::cmd_reg_genmove);
}

void Engine::set_show_board(bool enable)
{
    if (enable && ! m_show_board)
        log() << get_board();
    m_show_board = enable;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
