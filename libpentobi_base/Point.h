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

/** Point (coordinate of on-board field) for Blokus game variants. */
using Point = libboardgame_base::Point<1564, 56, 28, unsigned short>;

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_POINT_H
