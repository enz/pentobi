//-----------------------------------------------------------------------------
/** @file libpentobi_base/FullGrid.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_FULL_GRID_H
#define LIBPENTOBI_BASE_FULL_GRID_H

#include "Point.h"
#include "libboardgame_base/FullGrid.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

template<typename T>
class FullGrid
    : public libboardgame_base::FullGrid<Point, T>
{
public:
    explicit FullGrid()
    {
    }
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_FULL_GRID_H
