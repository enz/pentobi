//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/LocalValue.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_LOCAL_VALUE_H
#define LIBPENTOBI_MCTS_LOCAL_VALUE_H

#include <algorithm>
#include "libpentobi_base/AdjIterator.h"
#include "libpentobi_base/Board.h"
#include "libpentobi_base/PointList.h"

namespace libpentobi_mcts {

using namespace std;
using libboardgame_base::ArrayList;
using libpentobi_base::AdjIterator;
using libpentobi_base::Board;
using libpentobi_base::Color;
using libpentobi_base::ColorMove;
using libpentobi_base::Geometry;
using libpentobi_base::Grid;
using libpentobi_base::Move;
using libpentobi_base::PieceInfo;
using libpentobi_base::Point;
using libpentobi_base::PointList;

//-----------------------------------------------------------------------------

/** Classify moves for the playout policy to prefer local response moves.
    A local response move is a move that occupies attach points of recent
    opponent moves or points that are adjacent or second-order adjacent to
    them. If there is only one adjacent point to such an opponent attach
    point missing to make it a 1-hole, the missing adjacent point counts
    as an attach point. */
class LocalValue
{
public:
    /** Compute the local response value for a move. */
    class Compute
    {
    public:
        Compute()
            : m_value(0)
        { }

        /** Contruct and already add the first point. */
        Compute(Point p, const LocalValue& local_value)
            : m_value(local_value.m_point_value[p])
        { }

        /** Add a point of the move. */
        void add_move_point(Point p, const LocalValue& local_value)
        {
            m_value += local_value.m_point_value[p];
        }

        /** Does the move occupy any local points? */
        bool has_local() const
        {
            return m_value != 0;
        }

        /** Get the number of local opponent attach points occupied by this
            move. */
        unsigned get_nu_attach() const
        {
            return (m_value & 0xf00u) >> 8;
        }

        /** Does the move occupy any points adjacent to local opponent attach
            points? */
        bool has_adj_attach() const
        {
            return ((m_value & 0x0f0u) != 0);
        }

        /** Does the move occupy any points second-order adjacent to local
            opponent attach points? */
        bool has_adj_attach_2() const
        {
            return ((m_value & 0x00fu) != 0);
        }

    private:
        unsigned m_value;
    };

    friend class Compute;

    LocalValue();

    /** Find the attach points of the last opponent moves in a given position.
        @param bd The board. */
    void init(const Board& bd);

    /** Get local distance value.
        0: not local, 3: attach point of recent opponent move, 2: adjacant to
        such attach points, 1: second-order adjacant to such attach points */
    unsigned get_local_dist(Point p) const
    {
        unsigned val = m_point_value[p];
        if (val == 0)
            return 0;
        if ((val & 0xf00u) != 0)
            return 3;
        if ((val & 0x0f0u) != 0)
            return 2;
        return 1;
    }

private:
    Grid<unsigned> m_point_value;

    /** Points with point value greater zero. */
    PointList m_points;
};

inline void LocalValue::init(const Board& bd)
{
    // Clear the stored last opponent attach moves
    for (Point p: m_points)
        m_point_value[p] = 0;
    m_points.clear();

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

#endif // LIBPENTOBI_MCTS_LOCAL_VALUE_H
