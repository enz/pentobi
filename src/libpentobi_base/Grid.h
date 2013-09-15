//-----------------------------------------------------------------------------
/** @file libpentobi_base/Grid.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_GRID_H
#define LIBPENTOBI_BASE_GRID_H

#include "Point.h"
#include "Geometry.h"
#include "libboardgame_base/Grid.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

// This should become an alias template (not yet supported by MSVC)
template<typename T>
class Grid
    : public libboardgame_base::Grid<Point, T>
{
public:
    explicit Grid()
    {
    }
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_GRID_H
