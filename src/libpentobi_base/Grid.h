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
class Grid
    : public libboardgame_base::Grid<Point, T>
{
public:
    explicit Grid()
    {
    }

    Grid(unsigned int sz)
        : libboardgame_base::Grid<Point, T>(sz)
    {
    }

    Grid(unsigned int sz, const T& val)
        : libboardgame_base::Grid<Point, T>(sz, val)
    {
    }
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_GRID_H
