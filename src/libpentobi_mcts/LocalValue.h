//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/LocalValue.h
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_LOCAL_VALUE_H
#define LIBPENTOBI_MCTS_LOCAL_VALUE_H

#include <algorithm>
#include "libboardgame_util/Log.h"
#include "libpentobi_base/AdjIterator.h"
#include "libpentobi_base/Board.h"
#include "libpentobi_base/PointList.h"

namespace libpentobi_mcts {

using namespace std;
using libboardgame_base::ArrayList;
using libboardgame_util::log;
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

/** Classify playout moves to prefer local response moves.
    In the playout policy, local response moves are preferred. That is, a
    random move is chosen from the set of moves with the highest local response
    value. The value depends on the proximity to the last opponent moves and
    the number of attach points of those moves occupied. */
class LocalValue
{
public:
    /** Compute the local response value for a move. */
    class Compute
    {
    public:
        Compute(Point p, const LocalValue& local_value);

        /** Add a point of the move. */
        void add_move_point(Point p, const LocalValue& local_value);

        /** Return the value. */
        unsigned get() const;

        /** Return upper bound on the value.
            Faster than get() and often good enough to know that we
            already have a move with a higher value. */
        unsigned get_upper_bound() const;

    private:
        unsigned m_value;
    };

    friend class Compute;

    /** Find the attach points of the last opponent moves in a given position.
        @param bd The board. */
    void init(const Board& bd);

    /** Clear the stored last opponent attach moves. */
    void clear();

private:
    Grid<unsigned> m_point_value;

    /** Points with point value greater zero. */
    PointList m_points;
};

inline LocalValue::Compute::Compute(Point p, const LocalValue& local_value)
{
    m_value = local_value.m_point_value[p];
}

inline void LocalValue::Compute::add_move_point(Point p,
                                                const LocalValue& local_value)
{
    m_value += local_value.m_point_value[p];
}

inline unsigned LocalValue::Compute::get() const
{
    // The bit ranges used in the value work only as long as there are not more
    // than 0x10 points covered by a piece.
    static_assert(PieceInfo::max_size < 0x10, "");
    if (m_value == 0)
        return 0;
    if (m_value < 0x010u)
        // Only 2nd-order adjacent to opponent attach point. Don't care how
        // many
        return 0x001u;
    // Ignore 2nd-order adj. to opp. attach point if we have opp. attach
    // points or adj. to opp. attach point. Only care if we have any adj. to
    // opp. attach points, not how many.
    if ((m_value & 0x0f0u) != 0)
        return (m_value & 0xf00u) + 0x010u;
    else
        return m_value & 0xf00u;
}

inline unsigned LocalValue::Compute::get_upper_bound() const
{
    return m_value;
}

inline void LocalValue::clear()
{
    for (auto i = m_points.begin(); i != m_points.end(); ++i)
        m_point_value[*i] = 0;
    m_points.clear();
}

inline void LocalValue::init(const Board& bd)
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

#endif // LIBPENTOBI_MCTS_LOCAL_VALUE_H
