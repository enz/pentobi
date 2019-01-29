//-----------------------------------------------------------------------------
/** @file libpentobi_base/Grid.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_GRID_H
#define LIBPENTOBI_BASE_GRID_H

#include "Point.h"
#include "libboardgame_base/Grid.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

template<typename T>
using Grid = libboardgame_base::Grid<Point, T>;

template<typename T>
using GridExt = libboardgame_base::GridExt<Point, T>;

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_GRID_H
