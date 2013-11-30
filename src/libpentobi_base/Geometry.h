//-----------------------------------------------------------------------------
/** @file libpentobi_base/Geometry.h
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_GEOMETRY_H
#define LIBPENTOBI_BASE_GEOMETRY_H

#include "Point.h"
#include "libboardgame_base/Geometry.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

typedef libboardgame_base::Geometry<libpentobi_base::Point> Geometry;

typedef Geometry::Iterator GeometryIterator;

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_GEOMETRY_H
