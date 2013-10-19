//-----------------------------------------------------------------------------
/** @file libpentobi_base/Grid.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_GRID_H
#define LIBPENTOBI_BASE_GRID_H

#include "Point.h"
#include "libboardgame_base/Grid.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

template<typename T>
using Grid = libboardgame_base::Grid<Point, T>;

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_GRID_H
