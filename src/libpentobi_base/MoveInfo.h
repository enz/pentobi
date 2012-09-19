//-----------------------------------------------------------------------------
/** @file libpentobi_base/MoveInfo.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MOVE_INFO_H
#define LIBPENTOBI_BASE_MOVE_INFO_H

#include "MovePoints.h"
#include "Piece.h"
#include "PieceInfo.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

struct MoveInfo
{
    Piece piece;

    MovePoints points;
};

//-----------------------------------------------------------------------------

typedef ArrayList<Point,PieceInfo::max_adj,unsigned short> AdjPoints;

//-----------------------------------------------------------------------------

typedef ArrayList<Point,PieceInfo::max_attach,unsigned short> AttachPoints;

//-----------------------------------------------------------------------------

/** Less frequently accessed move info.
    Stored separately from MoveInfo to improve CPU cache performance. */
struct MoveInfoExt
{
    AdjPoints adj_points;

    AttachPoints attach_points;

    bool breaks_symmetry;

    Move symmetric_move;

    Point center;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_BASE_MOVE_INFO_H
