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

class MoveInfo
{
public:
    MoveInfo()
    {
    }

    MoveInfo(Piece piece, const MovePoints& points)
    {
        m_piece = static_cast<uint8_t>(piece.to_int());
        m_size = 0;
        for (auto p : points)
            m_points[m_size++] = p;
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
    Point points[PieceInfo::max_adj_attach];

    uint8_t size_attach_points;

    uint8_t size_adj_points;

    void init(const ArrayList<Point, PieceInfo::max_adj>& adj_points,
              const ArrayList<Point, PieceInfo::max_attach>& attach_points)
    {
        size_attach_points = static_cast<uint8_t>(attach_points.size());
        size_adj_points = static_cast<uint8_t>(adj_points.size());
        LIBBOARDGAME_ASSERT(size_attach_points + size_adj_points
                            <= PieceInfo::max_adj_attach);
        Point* i = points;
        for (Point p : attach_points)
            *(i++) = p;
        for (Point p : adj_points)
            *(i++) = p;
    }

    const Point* begin_attach() const
    {
        return points;
    }

    const Point* end_attach() const
    {
        return points + size_attach_points;
    }

    const Point* begin_adj() const
    {
        return end_attach();
    }

    const Point* end_adj() const
    {
        return begin_adj() + size_adj_points;
    }
};

//-----------------------------------------------------------------------------

/** Leeast frequently accessed move info.
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
