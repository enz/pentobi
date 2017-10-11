//-----------------------------------------------------------------------------
/** @file libpentobi_base/MovePoints.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MOVE_POINTS_H
#define LIBPENTOBI_BASE_MOVE_POINTS_H

#include "PieceInfo.h"
#include "Point.h"
#include "libboardgame_util/ArrayList.h"

namespace libpentobi_base {

using libboardgame_util::ArrayList;

//-----------------------------------------------------------------------------

using MovePoints = ArrayList<Point, PieceInfo::max_size, unsigned short>;

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_BASE_MOVE_POINTS_H
