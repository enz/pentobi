//-----------------------------------------------------------------------------
/** @file libboardgame_base/GeometryUtil.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_GEOMETRY_UTIL_H
#define LIBBOARDGAME_BASE_GEOMETRY_UTIL_H

#include "Geometry.h"

#include "libboardgame_util/Log.h"

namespace libboardgame_base {
namespace geometry_util {

using libboardgame_util::log;

//-----------------------------------------------------------------------------

/** Shift a list of points as close to the (0,0) point as possible.
    This will minimize the minumum x and y coordinates. The function also
    returns the width and height of the bounding box and the offset that was
    subtracted from the points for the shifting.
    @note This transformation does not preserve point types. If the original
    list was compatible with the point types on the board, the new point type of
    (0,0) will be Geometry::get_point_type(offset).
    @tparam T An iterator over a container containing CoordPoint element. */
template<typename P, typename T>
void normalize_offset(const Geometry<P>& geometry, T begin, T end,
                      unsigned int& width, unsigned int& height,
                      CoordPoint& offset)
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
    width = max_x - min_x + 1;
    height = max_y - min_y + 1;
    offset = CoordPoint(min_x, min_y);
    for (auto i = begin; i != end; ++i)
        *i -= offset;
}

/** Shift a list of points that are not compatible with the point types used
    in the geometry such that they match it.
    The points are shifted in a minimal positive direction to match the
    types, x-direction is preferred.
    @tparam T An iterator over a container containing CoordPoint element.
    @param geometry The geometry.
    @param begin The beginning of the list of points.
    @param end The end of the list of points.
    @param point_type The point type of (0,0) in the list of points. */
template<typename P, typename T>
void type_match_shift(const Geometry<P>& geometry, T begin, T end,
                         unsigned int point_type)
{
    CoordPoint type_match_shift(0, 0); // Init to avoid compiler warning
    bool found = false;
    for (unsigned int y = 0; ! found && y < geometry.get_period_y(); ++y)
        for (unsigned int x = 0; ! found && x < geometry.get_period_x(); ++x)
            if (geometry.get_point_type(x, y) == point_type)
            {
                type_match_shift = CoordPoint(x, y);
                found = true;
            }
    LIBBOARDGAME_ASSERT(found);
    for (auto i = begin; i != end; ++i)
        *i += type_match_shift;
}

//-----------------------------------------------------------------------------

} // namespace geometry_util
} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_GEOMETRY_UTIL_H
