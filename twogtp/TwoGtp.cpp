//-----------------------------------------------------------------------------
/** @file twogtp/TwoGtp.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "TwoGtp.h"

#include "libboardgame_sgf/Writer.h"
#include "libboardgame_base/Log.h"
#include "libpentobi_base/ScoreUtil.h"

using libboardgame_sgf::Writer;
using libpentobi_base::get_multiplayer_result;
using libpentobi_base::Move;
using libpentobi_base::ScoreType;

//-----------------------------------------------------------------------------

TwoGtp::TwoGtp(const string& black, const string& white, Variant variant,
               unsigned nu_games, Output& output, bool quiet,
               const string& log_prefix, bool fast_open)
    : m_quiet(quiet),
      m_fast_open(fast_open),
      m_variant(variant),
      m_nu_games(nu_games),
      m_bd(variant),
      m_output(output),
      m_black(black),
      m_white(white)
{
    if (! m_quiet)
    {
        m_black.enable_log(log_prefix + "B");
        m_white.enable_log(log_prefix + "W");
    }
    if (get_nu_colors(m_variant) == 2)
    {
        m_colors[0] = "b";
        m_colors[1] = "w";
    }
    else
    {
        m_colors[0] = "1";
        m_colors[1] = "2";
        m_colors[2] = "3";
        m_colors[3] = "4";
    }
}

float TwoGtp::get_result(unsigned player_black)
{
    float result;
    auto nu_players = m_bd.get_nu_players();
    if (nu_players == 2)
    {
        auto score = m_bd.get_score_twoplayer(Color(0));
        if (score > 0)
            result = 1;
        else if (score < 0)
            result = 0;
        else
            result = 0.5;
        if (player_black != 0)
            result = 1 - result;
    }
    else
    {
        array<ScoreType, Color::range> points;
        for (Color::IntType i = 0; i < m_bd.get_nu_colors(); ++i)
            points[i] = m_bd.get_points(Color(i));
        array<float, Color::range> player_result;
        get_multiplayer_result(nu_players, points, player_result,
                               m_bd.get_break_ties());
        result = player_result[player_black];
    }
    return result;
}

void TwoGtp::play_game(unsigned game_number)
{
    if (! m_quiet)
        LIBBOARDGAME_LOG("================================================\n"
                         "Game ", game_number, "\n"
                         "================================================");
    m_bd.init();
    send_both("clear_board");
    auto cpu_black = send_cputime(m_black);
    auto cpu_white = send_cputime(m_white);
    unsigned nu_players = m_bd.get_nu_players();
    unsigned player_black = game_number % nu_players;
    bool resign = false;
    ostringstream sgf_string;
    Writer sgf(sgf_string);
    sgf.set_indent(-1);
    sgf.begin_tree();
    sgf.begin_node();
    sgf.write_property("GM", to_string(m_variant));
    sgf.write_property("GN", game_number);
    sgf.end_node();
    array<bool, Board::max_moves> is_real_move;
    unsigned player;
    while (! m_bd.is_game_over())
    {
        auto to_play = m_bd.get_effective_to_play();
        if (m_variant == Variant::classic_3 && to_play == Color(3))
            player = m_bd.get_alt_player();
        else
            player = to_play.to_int() % nu_players;
        auto& player_connection = (player == player_black ? m_black : m_white);
        auto& other_connection = (player == player_black ? m_white : m_black);
        auto color = m_colors[to_play.to_int()];
        Move mv;
        if (m_fast_open
                && m_output.generate_fast_open_move(player == player_black,
                                                    m_bd, to_play, mv))
        {
            is_real_move[m_bd.get_nu_moves()] = false;
            LIBBOARDGAME_LOG("Playing fast opening move");
            player_connection.send("play " + color + " " + m_bd.to_string(mv));
        }
        else
        {
            is_real_move[m_bd.get_nu_moves()] = true;
            auto response = player_connection.send("genmove " + color);
            if (response == "resign")
            {
                resign = true;
                break;
            }
            if (! m_bd.from_string(mv, response))
                throw runtime_error("invalid move");
        }
        sgf.begin_node();
        sgf.write_property(string(1, static_cast<char>(toupper(color[0]))),
                           m_bd.to_string(mv));
        sgf.end_node();
        if (mv.is_null() || ! m_bd.is_legal(to_play, mv))
            throw runtime_error("invalid move: " + m_bd.to_string(mv));
        m_bd.play(to_play, mv);
        other_connection.send("play " + color + " " + m_bd.to_string(mv));
    }
    cpu_black = send_cputime(m_black) - cpu_black;
    cpu_white = send_cputime(m_white) - cpu_white;
    float result;
    if (resign)
    {
        if (nu_players > 2)
            throw runtime_error("resign only allowed in two-player variants");
        result = (player == player_black ? 0 : 1);
    }
    else
        result = get_result(player_black);
    sgf.end_tree();
    sgf_string << '\n';
    m_output.add_result(game_number, result, m_bd, player_black, cpu_black,
                        cpu_white, sgf_string.str(), is_real_move);
}

void TwoGtp::run()
{
    send_both(string("set_game ") + to_string(m_variant));
    while (! m_output.check_sentinel())
    {
        unsigned n = m_output.get_next();
        if (n >= m_nu_games)
            break;
        play_game(n);
    }
    send_both("quit");
}

void TwoGtp::send_both(const string& cmd)
{
    m_black.send(cmd);
    m_white.send(cmd);
}

double TwoGtp::send_cputime(GtpConnection& gtp_connection)
{
    string response = gtp_connection.send("cputime");
    istringstream in(response);
    double cputime;
    in >> cputime;
    if (! in)
        throw runtime_error("invalid response to cputime: " + response);
    return cputime;
}

//-----------------------------------------------------------------------------
