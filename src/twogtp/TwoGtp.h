//-----------------------------------------------------------------------------
/** @file twogtp/TwoGtp.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef TWOGTP_TWOGTP_H
#define TWOGTP_TWOGTP_H

#include <array>
#include "GtpConnection.h"
#include "OutputFile.h"
#include "libpentobi_base/Board.h"

using namespace std;
using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

class TwoGtp
{
public:
    TwoGtp(const string& black, const string& white, Variant variant,
           unsigned nu_games, OutputFile& output, bool quiet,
           const string& log_prefix);

    void run();

private:
    bool m_quiet;

    Variant m_variant;

    unsigned m_nu_games;

    Board m_bd;

    OutputFile& m_output;

    GtpConnection m_black;

    GtpConnection m_white;

    array<const char*, Color::range> m_colors;

    float get_result(unsigned player_black);

    void play_game(unsigned game_number);

    void send_both(const string& cmd);

    double send_cputime(GtpConnection& gtp_connection);
};

//-----------------------------------------------------------------------------

#endif // TWOGTP_TWOGTP_H
