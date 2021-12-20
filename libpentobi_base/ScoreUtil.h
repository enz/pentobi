//-----------------------------------------------------------------------------
/** @file libpentobi_base/ScoreUtil.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_SCORE_UTIL_H
#define LIBPENTOBI_BASE_SCORE_UTIL_H

#include <algorithm>
#include <array>
#include "Color.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** Convert the result of a multi-player game into a comparable number.
    This generalizes the game result of a two-player game (0,0.5,1 for
    loss/tie/win) for a game with n \> 2 players. The points are sorted in
    ascending order. Each rank r_i (i in 0..n-1) is assigned a value of
    r_i/(n-1). If multiple players have the same points, the result value is
    the average of all ranks with these points. So being the single winner
    still gives the result 1 and being the single loser the result 0. Being the
    single winner is better than sharing the best rank, which is better than
    getting the second rank, etc.
    @return The game result for each player. */
template<typename FLOAT>
void get_multiplayer_result(unsigned nu_players,
                            array<ScoreType, Color::range>& points,
                            array<FLOAT, Color::range>& result,
                            bool break_ties)
{
    array<ScoreType, Color::range> sorted;
    for (Color::IntType i = 0; i < nu_players; ++i)
    {
        if (break_ties)
            // Favor later player. The adjustment must be smaller than the
            // smallest difference in points (0.5 for GembloQ).
            points[i] +=  0.001f * i;
        sorted[i] = points[i];
    }
    sort(sorted.begin(), sorted.begin() + nu_players);
    for (Color::IntType i = 0; i < nu_players; ++i)
    {
        FLOAT sum = 0;
        FLOAT n = 0;
        FLOAT float_j = 0;
        FLOAT factor = 1 / FLOAT(nu_players - 1);
        for (unsigned j = 0; j < nu_players; ++j)
        {
            if (sorted[j] == points[i])
            {
                sum += factor * float_j;
                ++n;
            }
            ++float_j;
        }
        result[i] = sum / n;
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_SCORE_UTIL_H
