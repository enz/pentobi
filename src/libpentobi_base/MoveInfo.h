//-----------------------------------------------------------------------------
/** @file libpentobi_base/MoveInfo.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MOVE_INFO_H
#define LIBPENTOBI_BASE_MOVE_INFO_H

#include <algorithm>
#include "Move.h"
#include "MovePoints.h"
#include "Piece.h"
#include "PieceInfo.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Most frequently accessed move info.
    Contains the points and the piece of the move. If the point list is smaller
    than PieceInfo::max_size, values above end() up to PieceInfo::max_size may
    be accessed and contain Point::null() to allow loop unrolling. */
class MoveInfo
{
public:
    MoveInfo() = default;

    MoveInfo(Piece piece, const MovePoints& points)
    {
        m_piece = static_cast<uint8_t>(piece.to_int());
        m_size = static_cast<uint8_t>(points.size());
        for (MovePoints::IntType i = 0; i < MovePoints::max_size; ++i)
            m_points[i] = points.get_unchecked(i);
    }

    const Point* begin() const
    {
        return m_points;
    }

    const Point* end() const
    {
        return m_points + m_size;
    }

    uint8_t size() const
    {
        return m_size;
    }

    Piece get_piece() const
    {
        return Piece(m_piece);
    }

    bool contains(Point p) const
    {
        return find(begin(), end(), p) != end();
    }

private:
    uint8_t m_piece;

    uint8_t m_size;

    Point m_points[PieceInfo::max_size];
};

//-----------------------------------------------------------------------------

/** Less frequently accessed move info.
    Stored separately from MoveInfo to improve CPU cache performance. */
struct MoveInfoExt
{
    /** Concatenated list of adjacent and attach points. */
    Point points[PieceInfo::max_adj_attach];

    uint8_t size_attach_points;

    uint8_t size_adj_points;

    const Point* begin_adj() const
    {
        return points;
    }

    const Point* end_adj() const
    {
        return points + size_adj_points;
    }

    const Point* begin_attach() const
    {
        return end_adj();
    }

    const Point* end_attach() const
    {
        return begin_attach() + size_attach_points;
    }

};

//-----------------------------------------------------------------------------

/** Least frequently accessed move info.
    Stored separately from MoveInfo to improve CPU cache performance. */
struct MoveInfoExt2
{
    /** Whether the move breaks rotational symmetry of the board.
        Currently not initialized for classic and trigon_3 board types because
        enforced rotational-symmetric draws are not used in the MCTS search on
        these boards (trigon_3 has no 2-player game variant and classic_2
        currently only supports colored starting points, which makes rotational
        draws impossible. */
    bool breaks_symmetry;

    /** The rotational-symmetric counterpart to this move.
        Currently not initialized for classic and trigon_3 board types (see
        comment at breaks_symmetry. */
    Move symmetric_move;

    Point label_pos;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_BASE_MOVE_INFO_H
