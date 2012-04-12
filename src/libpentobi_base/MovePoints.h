//-----------------------------------------------------------------------------
/** @file libpentobi_base/MovePoints.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MOVE_POINTS_H
#define LIBPENTOBI_BASE_MOVE_POINTS_H

#include "PieceInfo.h"
#include "Point.h"
#include "libboardgame_util/ArrayList.h"

namespace libpentobi_base {

using libboardgame_util::ArrayList;

//-----------------------------------------------------------------------------

typedef ArrayList<Point, PieceInfo::max_size> MovePoints;

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_BASE_MOVE_POINTS_H
