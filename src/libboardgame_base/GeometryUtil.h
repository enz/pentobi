//-----------------------------------------------------------------------------
/** @file libboardgame_base/GeometryUtil.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_GEOMETRY_UTIL_H
#define LIBBOARDGAME_BASE_GEOMETRY_UTIL_H

#include "Geometry.h"

namespace libboardgame_base {
namespace geometry_util {

//-----------------------------------------------------------------------------

/** Shift a list of points as close to the (0,0) point as possible.
    This will minimize the minimum x and y coordinates. The function also
    returns the width and height of the bounding box and the offset that was
    subtracted from the points for the shifting.
    @note This transformation does not preserve point types. If the original
    list was compatible with the point types on the board, the new point type of
    (0,0) will be Geometry::get_point_type(offset).
    @tparam T An iterator over a container containing CoordPoint element. */
template<typename T>
void normalize_offset(T begin, T end, unsigned& width, unsigned& height,
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
    width = static_cast<unsigned>(max_x - min_x + 1);
    height = static_cast<unsigned>(max_y - min_y + 1);
    offset = CoordPoint(min_x, min_y);
    for (auto i = begin; i != end; ++i)
        *i -= offset;
}

/** Get an offset to shift points that are not compatible with the point types
    used in the geometry.
    The offset shifts points in a minimal positive direction to match the
    types, x-direction is preferred.
    @param geo
    @param point_type The point type of (0, 0) of the coordinate system used by
    the points. */
template<typename P>
CoordPoint type_match_offset(const Geometry<P>& geo, unsigned point_type)
{
    for (unsigned y = 0; y < geo.get_period_y(); ++y)
        for (unsigned x = 0; x < geo.get_period_x(); ++x)
            if (geo.get_point_type(x, y) == point_type)
                return CoordPoint(x, y);
    LIBBOARDGAME_ASSERT(false);
    return CoordPoint(0, 0);
}

/** Apply type_match_offset() to a list of points.
    @tparam T An iterator over a container containing CoordPoint elements.
    @param geo The geometry.
    @param begin The beginning of the list of points.
    @param end The end of the list of points.
    @param point_type The point type of (0,0) in the list of points. */
template<typename P, typename T>
void type_match_shift(const Geometry<P>& geo, T begin, T end,
                      unsigned point_type)
{
    CoordPoint offset = type_match_offset(geo, point_type);
    for (auto i = begin; i != end; ++i)
        *i += offset;
}

//-----------------------------------------------------------------------------

} // namespace geometry_util
} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_GEOMETRY_UTIL_H
