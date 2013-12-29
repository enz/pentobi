//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/LocalValue.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "LocalValue.h"

namespace libpentobi_mcts {

//-----------------------------------------------------------------------------

LocalValue::LocalValue()
{
    m_point_value.fill_all(0);
}

void LocalValue::clear()
{
    for (auto i = m_points.begin(); i != m_points.end(); ++i)
        m_point_value[*i] = 0;
    m_points.clear();
}

void LocalValue::init(const Board& bd)
{
    if (! m_points.empty())
        clear();
    Color to_play = bd.get_to_play();
    Color second_color = bd.get_second_color(to_play);
    auto& geo = bd.get_geometry();
    unsigned move_number = bd.get_nu_moves();
    // Consider last 3 moves for local points (i.e. last 2 opponent moves in
    // two-player variants)
    for (unsigned i = 0; i < 3; ++i)
    {
        if (move_number == 0)
            return;
        --move_number;
        ColorMove move = bd.get_move(move_number);
        Color c = move.color;
        if (c == to_play || c == second_color)
            continue;
        auto mv = move.move;
        if (mv.is_pass())
            continue;
        auto& is_forbidden = bd.is_forbidden(c);
        auto& info_ext = bd.get_move_info_ext(mv);
        auto j = info_ext.begin_attach();
        auto end = info_ext.end_attach();
        do
        {
            if (! is_forbidden[*j])
            {
                if (m_point_value[*j] == 0)
                    m_points.push_back(*j);
                // Opponent attach point
                m_point_value[*j] = 0x100u;
                unsigned nu_adj = 0;
                for (AdjIterator k(geo, *j); k; ++k)
                    if (! is_forbidden[*k])
                    {
                        ++nu_adj;
                        if (m_point_value[*k] < 0x010u)
                        {
                            if (m_point_value[*k] == 0)
                                m_points.push_back(*k);
                            // Adjacent to opp. attach point
                            m_point_value[*k] = 0x010u;
                            for (AdjIterator l(geo, *k); l; ++l)
                                if (! is_forbidden[*l]
                                    && m_point_value[*l] == 0)
                                {
                                    m_points.push_back(*l);
                                    // 2nd-order adj. to opp. attach point
                                    m_point_value[*l] = 0x001u;
                                }
                        }
                    }
                // If occupying the attach point is forbidden for us but there
                // is only one adjacent point missing to make it a 1-point hole
                // for the opponent, then occupying this adjacent point is
                // (almost) as good as occupying the attach point. (This is
                // done only for 1-point holes that are forbidden for to_play.)
                if (nu_adj == 1 && bd.is_forbidden(*j, to_play))
                    for (AdjIterator k(geo, *j); k; ++k)
                        if (! is_forbidden[*k])
                            m_point_value[*k] = 0x100u;
            }
        }
        while (++j != end);
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
