//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/LocalPoints.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_LOCAL_POINTS_H
#define LIBPENTOBI_MCTS_LOCAL_POINTS_H

#include "libpentobi_base/Board.h"
#include "libpentobi_base/Grid.h"

namespace libpentobi_mcts {

using libpentobi_base::Board;
using libpentobi_base::BoardConst;
using libpentobi_base::Color;
using libpentobi_base::GridExt;
using libpentobi_base::Point;
using libpentobi_base::PointList;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

/** Find attach points of recent opponent moves on the board. */
class LocalPoints
{
public:
    LocalPoints();

    template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
    void init(const Board& bd);

    bool contains(Point p) const { return m_is_local[p]; }

private:
    GridExt<bool> m_is_local;

    /** Points in m_is_local with value true. */
    PointList m_points;
};

template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
inline void LocalPoints::init(const Board& bd)
{
    for (Point p : m_points)
        m_is_local[p] = false;
    unsigned nu_local = 0;
    Color to_play = bd.get_to_play();
    Color second_color;
    if (bd.get_variant() == Variant::classic_3 && to_play.to_int() == 3)
        second_color = Color(bd.get_alt_player());
    else
        second_color = bd.get_second_color(to_play);
    auto& moves = bd.get_moves();
    auto move_info_ext_array = bd.get_board_const().get_move_info_ext_array();
    // Consider last 3 moves for local points (i.e. last 2 opponent moves in
    // two-color variants)
    auto end = moves.end();
    auto begin = (end - moves.begin() < 3 ? moves.begin() : end - 3);
    for (auto i = begin; i != end; ++i)
    {
        Color c = i->color;
        if (c == to_play || c == second_color)
            continue;
        auto mv = i->move;
        auto& is_forbidden = bd.is_forbidden(c);
        auto& info_ext = BoardConst::get_move_info_ext<MAX_ADJ_ATTACH>(
                    mv, move_info_ext_array);
        auto j = info_ext.begin_attach();
        auto end = info_ext.end_attach();
        do
            if (! is_forbidden[*j] && ! m_is_local[*j])
            {
                m_points.get_unchecked(nu_local++) = *j;
                m_is_local[*j] = true;
            }
        while (++j != end);
    }
    m_points.resize(nu_local);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_LOCAL_POINTS_H
