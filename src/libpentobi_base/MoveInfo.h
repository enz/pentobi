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

    ArrayList<Point, Piece::max_attach> attach_points;

    bool breaks_symmetry;

    Move symmetric_move;

    Point center;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_BASE_MOVE_INFO_H
