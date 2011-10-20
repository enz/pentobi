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

typedef libboardgame_base::Point<20, 20, SpreadsheetStringRep> Point;

typedef Point::Direction Direction;

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_POINT_H
