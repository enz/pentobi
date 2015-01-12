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

using libpentobi_base::BoardIterator;
using libpentobi_base::Color;
using libpentobi_base::ColorMove;
using libpentobi_base::Point;
using libpentobi_base::PointState;

//-----------------------------------------------------------------------------

namespace {

/** Return the symmetric point state for symmetry detection.
    Only used for Variant::duo. Returns the other color or empty, if the
    given point state is empty. */
PointState get_symmetric_state(Color c)
{
    if (c == Color(0))
        return Color(1);
    else if (c == Color(1))
        return Color(0);
    else if (c == Color(2))
        return Color(3);
    else
    {
        LIBBOARDGAME_ASSERT(c == Color(3));
        return Color(2);
    }
}

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
        for (BoardIterator i(bd); i; ++i)
        {
            PointState s1 = bd.get_point_state(*i);
            if (! s1.is_empty())
            {
                Point symm_p = symmetric_points[*i];
                PointState s2 = bd.get_point_state(symm_p);
                if (s2 != get_symmetric_state(s1.to_color()))
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
        auto& info = bd.get_move_info(last_mv.move);
        for (BoardIterator i(bd); i; ++i)
        {
            PointState s1 = bd.get_point_state(*i);
            if (! s1.is_empty())
            {
                Point symm_p = symmetric_points[*i];
                PointState s2 = bd.get_point_state(symm_p);
                if (s2 != get_symmetric_state(s1.to_color()))
                    if (! (info.contains(*i) && s2.is_empty()))
                        return true;
            }
        }
    }
    return false;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
