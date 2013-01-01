//-----------------------------------------------------------------------------
/** @file libpentobi_base/Point.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_POINT_H
#define LIBPENTOBI_BASE_POINT_H

#include "libboardgame_base/Point.h"
#include "libboardgame_base/SpreadsheetStringRep.h"

namespace libpentobi_base {

using libboardgame_base::SpreadsheetStringRep;

//-----------------------------------------------------------------------------

/** Point (coordinate of on-board field) for Blokus game variants.
    Supports RectGeometry up to size 20 and TrigonGeometry up to edge size 9. */
typedef libboardgame_base::Point<35,20,unsigned short,SpreadsheetStringRep,685>
    Point;

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_POINT_H
