//-----------------------------------------------------------------------------
/** @file twogtp/Output.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef TWOGTP_OUTPUT_H
#define TWOGTP_OUTPUT_H

#include <string>
#include <map>
#include <mutex>
#include "OutputTree.h"

//-----------------------------------------------------------------------------

/** Handles the output files of TwoGtp and their concurrent access. */
class Output
{
public:
    Output(Variant variant, const string& prefix, bool fastopen);

    ~Output();

    void add_result(unsigned n, float result, const Board& bd,
                    unsigned player_black, double cpu_black, double cpu_white,
                    const string& sgf,
                    array<bool, Board::max_game_moves>& is_real_move);

    unsigned get_next();

    bool check_sentinel();

    bool generate_fast_open_move(bool is_player_black, const Board& bd,
                                 Color to_play, Move& mv);

private:
    bool m_create_tree;

    unsigned m_next;

    int m_lock_fd;

    string m_prefix;

    mutex m_mutex;

    map<unsigned, string> m_games;

    OutputTree m_output_tree;
};

//-----------------------------------------------------------------------------

#endif // TWOGTP_OUTPUT_H
