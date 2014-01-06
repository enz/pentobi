//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/LocalValue.h
    @author Markus Enzenberger
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

/** Classify moves for the playout policy to prefer local response moves.
    A local response move is a move that occupies attach points of recent
    opponent moves or points that are adjacent or second-order adjacent to
    them. */
class LocalValue
{
public:
    /** Compute the local response value for a move. */
    class Compute
    {
    public:
        Compute(Point p, const LocalValue& local_value)
            : m_value(local_value.m_point_value[p])
        {
        }

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

    /** Clear the stored last opponent attach moves. */
    void clear();

private:
    Grid<unsigned> m_point_value;

    /** Points with point value greater zero. */
    PointList m_points;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_LOCAL_VALUE_H
