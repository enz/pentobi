//-----------------------------------------------------------------------------
/** @file twogtp/TwoGtp.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TwoGtp.h"

#include "libboardgame_sgf/Writer.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_util/StringUtil.h"
#include "libboardgame_util/Unused.h"
#include "libpentobi_base/ScoreUtil.h"

using libboardgame_sgf::Writer;
using libboardgame_util::log;
using libboardgame_util::trim;
using libpentobi_base::get_multiplayer_result;
using libpentobi_base::get_nu_players;

//-----------------------------------------------------------------------------

TwoGtp::TwoGtp(const string& black, const string& white, Variant variant,
               unsigned nu_games, OutputFile& output, bool quiet,
               const string& log_prefix)
    : m_quiet(quiet),
      m_variant(variant),
      m_nu_games(nu_games),
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

float TwoGtp::get_result(unsigned player_black, const string& final_score)
{
    unsigned nu_players = get_nu_players(m_variant);
    float result;
    if (nu_players == 2)
    {
        if (final_score.find("B+") != string::npos)
            result = 1;
        else if (final_score.find("W+") != string::npos)
            result = 0;
        else if (trim(final_score) == "0")
            result = 0.5;
        else
            throw Exception("invalid final_score: " + final_score);
        if (player_black != 0)
            result = 1 - result;
    }
    else
    {
        array<unsigned, Color::range> points;
        istringstream in(final_score);
        for (unsigned i = 0; i < nu_players; ++i)
        {
            in >> points[i];
            if (! in)
                throw Exception("invalid final_score: " + final_score);
        }
        array<float, Color::range> player_result;
        get_multiplayer_result(nu_players, points, player_result);
        result = player_result[player_black];
    }
    return result;
}

void TwoGtp::play_game(unsigned game_number)
{
    if (! m_quiet)
    {
        log("=====================================================");
        log("Game ", game_number);
        log("=====================================================");
    }
    send_both("clear_board");
    auto cpu_black = send_cputime(m_black);
    auto cpu_white = send_cputime(m_white);
    unsigned nu_players = get_nu_players(m_variant);
    unsigned nu_colors = get_nu_colors(m_variant);
    unsigned player_black = game_number % nu_players;
    m_has_moves.fill(true);
    unsigned nu_moves = 0;
    unsigned nu_passes = 0;
    bool resign = false;
    unsigned color_to_play = 0;
    unsigned player;
    ostringstream sgf_string;
    Writer sgf(sgf_string);
    sgf.set_indent(0);
    sgf.begin_tree();
    sgf.begin_node();
    sgf.write_property("GM", to_string(m_variant));
    sgf.write_property("GN", game_number);
    sgf.end_node();
    while (true)
    {
        if (m_variant == Variant::classic_3 && color_to_play == 3)
            player = nu_moves % 3;
        else
            player = color_to_play % nu_players;
        auto& player_connection = (player == player_black ? m_black : m_white);
        auto& other_connection = (player == player_black ? m_white : m_black);
        auto color = m_colors[color_to_play];
        auto move = player_connection.send(string("genmove ") + color);
        if (move == "resign")
        {
            resign = true;
            break;
        }
        if (move == "pass")
        {
            m_has_moves[color_to_play] = false;
            if (++nu_passes == nu_colors)
                break;
        }
        else
        {
            sgf.begin_node();
            sgf.write_property(string(1, toupper(color[0])), move);
            sgf.end_node();
            other_connection.send(string("play ") + color + " " + move);
            ++nu_moves;
        }
        do
            color_to_play = (color_to_play + 1) % nu_colors;
        while (! m_has_moves[color_to_play]);
    }
    cpu_black = send_cputime(m_black) - cpu_black;
    cpu_white = send_cputime(m_white) - cpu_white;
    float result;
    if (resign)
    {
        if (nu_players > 2)
            throw Exception("resign only allowed in two-player game variants");
        result = (player == player_black ? 0 : 1);
    }
    else
    {
        string final_score = m_black.send("final_score");
        result = get_result(player_black, final_score);
    }
    sgf.end_tree();
    m_output.add_result(game_number, result, nu_moves, player_black, cpu_black,
                        cpu_white, sgf_string.str());
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
        throw Exception("invalid response to cputime: " + response);
    return cputime;
}

//-----------------------------------------------------------------------------
