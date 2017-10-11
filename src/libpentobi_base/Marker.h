//-----------------------------------------------------------------------------
/** @file libpentobi_base/Marker.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MARKER_H
#define LIBPENTOBI_BASE_MARKER_H

#include "Point.h"
#include "libboardgame_base/Marker.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

using Marker = libboardgame_base::Marker<Point>;

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_MARKER_H
