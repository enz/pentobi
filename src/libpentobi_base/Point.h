//-----------------------------------------------------------------------------
/** @file libpentobi_base/Point.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_POINT_H
#define LIBPENTOBI_BASE_POINT_H

#include "libboardgame_base/Point.h"

//-----------------------------------------------------------------------------

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** Point (coordinate of on-board field) for Blokus game variants.
    Supports RectGeometry up to size 20, TrigonGeometry up to edge size 9,
    and NexosGeometry up to size 13. */
typedef libboardgame_base::Point<486, 35, 25, unsigned short> Point;

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_POINT_H
