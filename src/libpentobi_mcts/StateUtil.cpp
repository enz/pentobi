//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/StateUtil.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "StateUtil.h"

namespace libpentobi_mcts {

using libpentobi_base::Color;
using libpentobi_base::ColorMove;
using libpentobi_base::Point;
using libpentobi_base::PointState;

//-----------------------------------------------------------------------------

namespace {

array<Color, Color::range> symmetric_state =
    { Color(1), Color(0), Color(3), Color(2) };

} // namespace

//-----------------------------------------------------------------------------

bool check_symmetry_broken(const Board& bd,
                           const SymmetricPoints& symmetric_points)
{
    Color to_play = bd.get_to_play();
    if (to_play == Color(0) || to_play == Color(2))
    {
        // First player to play: the symmetry is broken if the position is
        // not symmetric.
        for (Point p : bd)
        {
            PointState s1 = bd.get_point_state(p);
            if (! s1.is_empty())
            {
                Point symm_p = symmetric_points[p];
                PointState s2 = bd.get_point_state(symm_p);
                if (s2 != symmetric_state[s1.to_int()])
                    return true;
            }
        }
    }
    else
    {
        // Second player to play: the symmetry is broken if the second player
        // cannot copy the first player's last move to make the position
        // symmetric again.
        unsigned nu_moves = bd.get_nu_moves();
        if (nu_moves == 0)
            // Don't try to handle the case if the second player has to play as
            // first move (e.g. in setup positions)
            return true;
        Color previous_color = bd.get_previous(to_play);
        ColorMove last_mv = bd.get_move(nu_moves - 1);
        if (last_mv.color != previous_color)
            // Don't try to handle non-alternating moves in board history
            return true;
        auto points = bd.get_move_points(last_mv.move);
        for (Point p : bd)
        {
            PointState s1 = bd.get_point_state(p);
            if (! s1.is_empty())
            {
                Point symm_p = symmetric_points[p];
                PointState s2 = bd.get_point_state(symm_p);
                if (s2 != symmetric_state[s1.to_int()])
                    if (! (points.contains(p) && s2.is_empty()))
                        return true;
            }
        }
    }
    return false;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
