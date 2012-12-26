//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/LocalValue.h */
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
using libpentobi_base::AttachPoints;
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
        Compute();

        /** Add a point of the move. */
        void add_move_point(Point p, const LocalValue& local_value);

        /** Finish the computation and return the value. */
        unsigned finish();

    private:
        unsigned m_value;
    };

    friend class Compute;

    /** Initialize geometry.
        Must be called before using this class and whenever the board geometry
        changes. */
    void init_geometry(const Geometry& geometry);

    /** Find the attach points of the last opponent moves in a given position.
        @param bd The board. Must have the same geometry as in the last call
        of init_geometry(). */
    void init(const Board& bd);

    /** Clear the stored last opponent attach moves. */
    void clear();

private:
    Grid<unsigned> m_point_value;

    /** Points with point value greater zero. */
    PointList m_points;
};

inline LocalValue::Compute::Compute()
{
    m_value = 0;
}

inline void LocalValue::Compute::add_move_point(Point p,
                                                const LocalValue& local_value)
{
    m_value += local_value.m_point_value[p];
}

inline unsigned LocalValue::Compute::finish()
{
    // The bit ranges used in the value work only as long as there are not more
    // than 0x10 points covered by a piece.
    static_assert(PieceInfo::max_size < 0x10, "");
    if (m_value == 0)
        return 0;
    if (m_value < 0x010u)
        // Only 2nd-order adjacent to opp. attach point. Don't care how many.
        return 0x001u;
    // Ignore 2nd-order adj. if we have attach points or adj. to attach points
    unsigned value = m_value & 0xff0u;
    if ((m_value & 0x0f0u) != 0)
        // Only care if we have any adj. to attach points, not how many
        return (value & 0xf00u) + 0x010u;
    else
        return value;
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
        Move mv = move.move;
        if (mv.is_pass())
            continue;
        const AttachPoints& attach_points
            = bd.get_move_info_ext(mv).attach_points;
        auto j = attach_points.begin();
        auto end = attach_points.end();
        LIBBOARDGAME_ASSERT(j != end);
        do
        {
            if (! bd.is_forbidden(*j, c))
            {
                if (m_point_value[*j] == 0)
                    m_points.push_back(*j);
                // Opponent attach point
                m_point_value[*j] = 0x100u;
                unsigned nu_adj = 0;
                for (AdjIterator k(bd, *j); k; ++k)
                    if (! bd.is_forbidden(*k, c))
                    {
                        ++nu_adj;
                        if (m_point_value[*k] < 0x010u)
                        {
                            if (m_point_value[*k] == 0)
                                m_points.push_back(*k);
                            // Adjacent to opp. attach point
                            m_point_value[*k] = 0x010u;
                            for (AdjIterator l(bd, *k); l; ++l)
                                if (! bd.is_forbidden(*l, c))
                                    if (m_point_value[*l] == 0)
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
                    for (AdjIterator k(bd, *j); k; ++k)
                        if (! bd.is_forbidden(*k, c))
                            m_point_value[*k] = 0x100u;
            }
            ++j;
        }
        while (j != end);
    }
}

inline void LocalValue::init_geometry(const Geometry& geometry)
{
    m_points.clear();
    m_point_value.init(geometry, 0);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_LOCAL_VALUE_H
