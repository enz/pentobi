//-----------------------------------------------------------------------------
/** @file libpentobi_base/Engine.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Engine.h"

#include "MoveList.h"
#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_sgf/SgfUtil.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_util/RandomGenerator.h"

namespace libpentobi_base {

using libboardgame_gtp::Failure;
using libboardgame_sgf::InvalidPropertyValue;
using libboardgame_sgf::TreeReader;
using libboardgame_sgf::util::get_last_node;
using libboardgame_util::log;
using libboardgame_util::ArrayList;
using libboardgame_util::RandomGenerator;

//-----------------------------------------------------------------------------

Engine::Engine(Variant variant)
    : m_accept_illegal(false),
      m_show_board(false),
      m_resign(true),
      m_game(variant),
      m_player(nullptr)
{
    add("all_legal", &Engine::cmd_all_legal);
    add("clear_board", &Engine::cmd_clear_board);
    add("final_score", &Engine::cmd_final_score);
    add("get_place", &Engine::cmd_get_place);
    add("loadsgf", &Engine::cmd_loadsgf);
    add("point_integers", &Engine::cmd_point_integers);
    add("move_info", &Engine::cmd_move_info);
    add("p", &Engine::cmd_p);
    add("param_base", &Engine::cmd_param_base);
    add("play", &Engine::cmd_play);
    add("set_game", &Engine::cmd_set_game);
    add("showboard", &Engine::cmd_showboard);
    add("undo", &Engine::cmd_undo);
}

void Engine::board_changed()
{
    if (m_show_board)
        log(get_board());
}

void Engine::cmd_all_legal(const Arguments& args, Response& response)
{
    auto& bd = get_board();
    unique_ptr<MoveList> moves(new MoveList);
    unique_ptr<MoveMarker> marker(new MoveMarker);
    bd.gen_moves(get_color_arg(args), *marker, *moves);
    for (Move mv : *moves)
        response << bd.to_string(mv, false) << '\n';
}

void Engine::cmd_clear_board()
{
    m_game.init();
    board_changed();
}

void Engine::cmd_final_score(Response& response)
{
    auto& bd = get_board();
    if (get_nu_players(bd.get_variant()) > 2)
    {
        for (Color c : bd.get_colors())
            response << bd.get_points(c) << ' ';
    }
    else
    {
        int score = bd.get_score(Color(0));
        if (score > 0)
            response << "B+" << score;
        else if (score < 0)
            response << "W+" << (-score);
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

void Engine::cmd_get_place(const Arguments& args, Response& response)
{
    auto& bd = get_board();
    unsigned place;
    bool isPlaceShared;
    bd.get_place(get_color_arg(args), place, isPlaceShared);
    response << place;
    if (isPlaceShared)
        response << " shared";
}

void Engine::cmd_loadsgf(const Arguments& args)
{
    args.check_size_less_equal(2);
    string file = args.get(0);
    int move_number = -1;
    if (args.get_size() == 2)
        move_number = args.parse_min<int>(1, 1) - 1;
    try
    {
        TreeReader reader;
        reader.read(file);
        auto tree = reader.get_tree_transfer_ownership();
        m_game.init(tree);
        const SgfNode* node = nullptr;
        if (move_number != -1)
            node = m_game.get_tree().get_node_before_move_number(move_number);
        if (! node)
            node = &get_last_node(m_game.get_root());
        m_game.goto_node(*node);
        board_changed();
    }
    catch (const Exception& e)
    {
        throw Failure(e.what());
    }
}

/** Return move info of a move given by its integer or string representation. */
void Engine::cmd_move_info(const Arguments& args, Response& response)
{
    auto& bd = get_board();
    Move mv;
    try
    {
        mv = Move(args.parse<Move::IntType>());
    }
    catch (const Failure&)
    {
        try
        {
            mv = bd.from_string(args.get());
        }
        catch (Exception&)
        {
            ostringstream msg;
            msg << "invalid argument '" << args.get()
                << "' (expected move or move ID)";
            throw Failure(msg.str());
        }
    }
    auto& geo = bd.get_geometry();
    auto& info = bd.get_move_info(mv);
    Piece piece = info.get_piece();
    auto& info_ext = bd.get_move_info_ext(mv);
    auto& info_ext_2 = bd.get_move_info_ext_2(mv);
    response
        << "\n"
        << "ID:     " << mv.to_int() << "\n"
        << "Piece:  " << static_cast<int>(piece.to_int())
        << " (" << bd.get_piece_info(piece).get_name() << ")\n"
        << "Points:";
    for (Point p : info)
        response << ' ' << geo.to_string(p);
    response
        << "\n"
        << "Adj:    ";
    for (auto i = info_ext.begin_adj(); i != info_ext.end_adj(); ++i)
        response << geo.to_string(*i) << " ";
    response
        << "\n"
        << "Attach: ";
    for (auto i = info_ext.begin_attach(); i != info_ext.end_attach(); ++i)
        response << geo.to_string(*i) << " ";
    response
        << "\n"
        << "BrkSym: " << info_ext_2.breaks_symmetry << "\n"
        << "SymMv:  " << bd.to_string(info_ext_2.symmetric_move);
}

void Engine::cmd_p(const Arguments& args)
{
    play(get_board().get_to_play(), args, 0);
}

void Engine::cmd_param_base(const Arguments& args, Response& response)
{
    if (args.get_size() == 0)
        response
            << "accept_illegal " << m_accept_illegal << '\n'
            << "resign " << m_resign << '\n';
    else
    {
        args.check_size(2);
        string name = args.get(0);
        if (name == "accept_illegal")
            m_accept_illegal = args.parse<bool>(1);
        else if (name == "resign")
            m_resign = args.parse<bool>(1);
        else
        {
            ostringstream msg;
            msg << "unknown parameter '" << name << "'";
            throw Failure(msg.str());
        }
    }
}

void Engine::cmd_play(const Arguments& args)
{
    play(get_color_arg(args, 0), args, 1);
}

void Engine::cmd_point_integers(Response& response)
{
    auto& geo = get_board().get_geometry();
    Grid<int> grid;
    for (Point p : geo)
        grid[p] = p.to_int();
    response << '\n' << grid.to_string(geo);
}

void Engine::cmd_reg_genmove(const Arguments& args, Response& response)
{
    RandomGenerator::set_global_seed_last();
    Move move = get_player().genmove(get_board(), get_color_arg(args));
    if (move.is_null())
        throw Failure("player failed to generate a move");
    response << get_board().to_string(move, false);
}

/** Set the game variant.
    Arguments:
    Blokus|Blokus Two-Player|Blokus Duo|Blokus Trigon|Blokus Trigon Two-Player
    <br>
    This command is similar to the command that is used by Quarry
    (http://home.gna.org/quarry/) to set a game at GTP engines that support
    multiple games. */
void Engine::cmd_set_game(const Arguments& args)
{
    Variant variant;
    if (! parse_variant(args.get_line(), variant))
        throw Failure("invalid argument");
    m_game.init(variant);
    board_changed();
}

void Engine::cmd_showboard(Response& response)
{
    response << '\n' << get_board();
}

void Engine::cmd_undo()
{
    auto& bd = get_board();
    if (bd.get_nu_moves() == 0)
        throw Failure("cannot undo");
    m_game.undo();
    board_changed();
}

void Engine::genmove(Color c, Response& response)
{
    auto& bd = get_board();
    auto& player = get_player();
    auto mv = player.genmove(bd, c);
    if (mv.is_null())
    {
        response << "pass";
        return;
    }
    if (! bd.is_legal(c, mv))
    {
        ostringstream msg;
        msg << "player generated illegal move: " << bd.to_string(mv);
        throw Failure(msg.str());
    }
    if (m_resign && player.resign())
    {
        response << "resign";
        return;
    }
    m_game.play(c, mv, true);
    response << bd.to_string(mv, false);
    board_changed();
}

Color Engine::get_color_arg(const Arguments& args) const
{
    if (args.get_size() > 1)
        throw Failure("too many arguments");
    return get_color_arg(args, 0);
}

Color Engine::get_color_arg(const Arguments& args, unsigned i) const
{
    string s = args.get_tolower(i);
    auto& bd = get_board();
    auto variant = bd.get_variant();
    switch (variant)
    {
    case Variant::classic:
    case Variant::classic_2:
    case Variant::classic_3:
    case Variant::trigon:
    case Variant::trigon_2:
    case Variant::trigon_3:
        if (s == "1" || s == "blue")
            return Color(0);
        if (s == "2" || s == "yellow")
            return Color(1);
        if (s == "3" || s == "red")
            return Color(2);
        if (s == "4" || s == "green")
            return Color(3);
        break;
    case Variant::duo:
    case Variant::junior:
        if (s == "blue" || s == "black" || s == "b")
            return Color(0);
        if (s == "green" || s == "white" || s == "w")
            return Color(1);
        break;
    }
    throw Failure("invalid color argument '" + s + "'");
}

PlayerBase& Engine::get_player() const
{
    if (! m_player)
        throw Failure("no player set");
    return *m_player;
}

void Engine::play(Color c, const Arguments& args, unsigned arg_move_begin)
{
    auto& bd = get_board();
    if (bd.get_nu_moves() >= Board::max_game_moves)
        throw Failure("too many moves");
    Move mv;
    try
    {
        if (arg_move_begin == 0)
            mv = bd.from_string(args.get_line());
        else
            mv = bd.from_string(args.get_remaining_line(arg_move_begin - 1));
    }
    catch (const Exception& e)
    {
        throw Failure(e.what());
    }
    if (mv.is_null())
        throw Failure("play pass not supported (anymore)");
    if (! m_accept_illegal && ! bd.is_legal(c, mv))
        throw Failure("illegal move");
    m_game.play(c, mv, true);
    board_changed();
}

void Engine::set_player(PlayerBase& player)
{
    m_player = &player;
    add("genmove", &Engine::cmd_genmove);
    add("g", &Engine::cmd_g);
    add("reg_genmove", &Engine::cmd_reg_genmove);
}

void Engine::set_show_board(bool enable)
{
    if (enable && ! m_show_board)
        log(get_board());
    m_show_board = enable;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
