//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/StateUtil.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "StateUtil.h"

namespace libpentobi_mcts {

using namespace std;
using libpentobi_base::Color;
using libpentobi_base::ColorMove;
using libpentobi_base::Geometry;
using libpentobi_base::Point;
using libpentobi_base::PointState;

//-----------------------------------------------------------------------------

namespace {

array<Color::IntType, Color::range> symmetric_state{ {1, 0, 3, 2} };

} // namespace

//-----------------------------------------------------------------------------

bool check_symmetry_broken(const Board& bd)
{
    LIBBOARDGAME_ASSERT(has_central_symmetry(bd.get_variant()));
    auto& symmetric_points = bd.get_board_const().get_symmetrc_points();
    Color to_play = bd.get_to_play();
    auto& geo = bd.get_geometry();
    // No need to iterator over the whole board when checking symmetry (this
    // makes the assumption that the symmetric points of the points in the
    // first half of the integer range are in the second half).
    Geometry::Iterator begin = geo.begin();
    LIBBOARDGAME_ASSERT(geo.get_range() % 2 == 0);
    Geometry::Iterator end(static_cast<Point::IntType>(geo.get_range() / 2));
#ifdef LIBBOARDGAME_DEBUG
    for (auto p = begin; p != end; ++p)
        LIBBOARDGAME_ASSERT(symmetric_points[*p].to_int() >= (*end).to_int());
#endif
    if (to_play == Color(0) || to_play == Color(2))
    {
        // First player to play: the symmetry is broken if the position is
        // not symmetric.
        for (auto p = begin; p != end; ++p)
        {
            PointState s1 = bd.get_point_state(*p);
            if (! s1.is_empty())
            {
                Point symm_p = symmetric_points[*p];
                PointState s2 = bd.get_point_state(symm_p);
                if (s2.to_int() != symmetric_state[s1.to_int()])
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
        for (Point p : points)
            if (! bd.get_point_state(symmetric_points[p]).is_empty())
                return true;
        for (auto p = begin; p != end; ++p)
        {
            PointState s1 = bd.get_point_state(*p);
            if (! s1.is_empty())
            {
                PointState s2 = bd.get_point_state(symmetric_points[*p]);
                if (s2.to_int() != symmetric_state[s1.to_int()]
                        && ! points.contains(*p))
                    return true;
            }
        }
    }
    return false;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
