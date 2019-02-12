//-----------------------------------------------------------------------------
/** @file libpentobi_base/MoveInfo.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MOVE_INFO_H
#define LIBPENTOBI_BASE_MOVE_INFO_H

#include "Move.h"
#include "MovePoints.h"
#include "Piece.h"
#include "PieceInfo.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** Most frequently accessed move info.
    Contains the points and the piece of the move. If the point list is smaller
    than MAX_SIZE, values above end() up to MAX_SIZE may be accessed and
    contain Point::null() to allow loop unrolling. The points correspond to
    PieceInfo::get_points(), which includes certain junction points in Nexos,
    see comment there.
    Since this is the most performance-critical data structure, it takes
    a template argument to make the space for move points not larger than
    needed in the current game variant. */
template<unsigned MAX_SIZE>
class MoveInfo
{
public:
    MoveInfo() = default;

    MoveInfo(Piece piece, const MovePoints& points)
    {
        m_piece = static_cast<uint_least8_t>(piece.to_int());
        m_size = static_cast<uint_least8_t>(points.size());
        for (MovePoints::IntType i = 0; i < MAX_SIZE; ++i)
            m_points[i] = points.get_unchecked(i);
    }

    const Point* begin() const { return m_points; }

    const Point* end() const { return m_points + m_size; }

    Piece get_piece() const { return Piece(m_piece); }

private:
    uint_least8_t m_piece;

    uint_least8_t m_size;

    Point m_points[MAX_SIZE];
};

//-----------------------------------------------------------------------------

/** Less frequently accessed move info.
    Stored separately from move points and move piece to improve CPU cache
    performance.
    Since this is a performance-critical data structure, it takes
    a template argument to make the space for move points not larger than
    needed in the current game variant.
    @tparam MAX_ADJ_ATTACH Maximum total number of attach points and adjacent
    points of a piece in the corresponding game variant. */
template<unsigned MAX_ADJ_ATTACH>
struct MoveInfoExt
{
    /** Concatenated list of adjacent and attach points. */
    Point points[MAX_ADJ_ATTACH];

    uint_least8_t size_attach_points;

    uint_least8_t size_adj_points;

    const Point* begin_adj() const { return points; }

    const Point* end_adj() const { return points + size_adj_points; }

    const Point* begin_attach() const { return end_adj(); }

    const Point* end_attach() const
    {
        return begin_attach() + size_attach_points;
    }
};

//-----------------------------------------------------------------------------

/** Least frequently accessed move info.
    Stored separately from move points and move piece to improve CPU cache
    performance. */
struct MoveInfoExt2
{
    /** Whether the move breaks rotational symmetry of the board.
        Currently not initialized for classic and trigon_3 board types because
        enforced rotational-symmetric draws are not used in the MCTS search on
        these boards (trigon_3 has no 2-player game variant and classic_2
        currently only supports colored starting points, which makes rotational
        draws impossible. */
    bool breaks_symmetry;

    uint_least8_t scored_points_size;

    /** The rotational-symmetric counterpart to this move.
        Only initialized for game variants that have rotational-symmetric
        boards and starting points. */
    Move symmetric_move;

    Point label_pos;

    /** The points of a move that contribute to the score, which excludes
        junction points in Nexos. */
    Point scored_points[PieceInfo::max_scored_size];


    const Point* begin_scored_points() const { return scored_points; }

    const Point* end_scored_points() const
    {
        return scored_points + scored_points_size;
    }
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_BASE_MOVE_INFO_H
