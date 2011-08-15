//-----------------------------------------------------------------------------
/** @file libpentobi_base/MoveInfo.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MOVE_INFO_H
#define LIBPENTOBI_BASE_MOVE_INFO_H

#include "MovePoints.h"
#include "Piece.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

struct MoveInfo
{
    unsigned int piece;

    MovePoints points;

    ArrayList<Point, Piece::max_adj> adj_points;

    ArrayList<Point, Piece::max_corner> corner_points;

    bool breaks_symmetry;

    Move symmetric_move;

    Point center;

    /** Euclidian distance to center rounded to the next smallest integer.
        Can be used for heuristics on 20x20 boards to prefer moves in the
        opening that go towards the center in a line as straight as possible. */
    unsigned int dist_to_center;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_BASE_MOVE_INFO_H
