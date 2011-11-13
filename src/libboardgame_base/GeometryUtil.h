//-----------------------------------------------------------------------------
/** @file libboardgame_base/GeometryUtil.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_GEOMETRY_UTIL_H
#define LIBBOARDGAME_BASE_GEOMETRY_UTIL_H

#include "Geometry.h"

namespace libboardgame_base {
namespace geometry_util {

//-----------------------------------------------------------------------------

/** Shift a list of points as close to the (0,0) point as possible.
    This will minimize the minumum x and y coordinates without changing the
    point types of the point. As a side effect, the function computes the width
    and height of the bounding box (includiding any remaining distance to (0,0)
    for preserving the point types).
    @tparam T An iterator over a container containing CoordPoint element.
    @see Geometry::get_point_type() */
template<typename P, typename T>
void normalize_offset(const Geometry<P>& geometry, T begin, T end,
                      unsigned int& width, unsigned int& height)
{
    int min_x = numeric_limits<int>::max();
    int min_y = numeric_limits<int>::max();
    int max_x = numeric_limits<int>::min();
    int max_y = numeric_limits<int>::min();
    for (auto i = begin; i != end; ++i)
    {
        if (i->x < min_x)
            min_x = i->x;
        if (i->x > max_x)
            max_x = i->x;
        if (i->y < min_y)
            min_y = i->y;
        if (i->y > max_y)
            max_y = i->y;
    }
    int dx = min_x % geometry.get_period_x();
    int dy = min_y % geometry.get_period_y();
    width = max_x - min_x + dx + 1;
    height = max_y - min_y + dy + 1;
    for (auto i = begin; i != end; ++i)
    {
        LIBBOARDGAME_ASSERT(geometry.get_point_type(i->x, i->y)
                            == geometry.get_point_type(i->x - min_x + dx,
                                                       i->y - min_y + dy));
        i->x = i->x - min_x + dx;
        i->y = i->y - min_y + dy;
    }
}

/** Shift a list of points as close to the (0,0) point as possible.
    Like normalize_offset(const Geometry&,T,T,unsigned int&,unsigned int&)
    but does not return width and height. */
template<typename P, typename T>
void normalize_offset(const Geometry<P>& geometry, T begin, T end)
{
    unsigned int width;
    unsigned int height;
    normalize_offset(geometry, begin, end, width, height);
}

//-----------------------------------------------------------------------------

} // namespace geometry_util
} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_GEOMETRY_UTIL_H
