//-----------------------------------------------------------------------------
/** @file libboardgame_base/tests/PointTransformTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_base/PointTransform.h"

#include "libboardgame_base/Point.h"
#include "libboardgame_base/RectGeometry.h"
#include "libboardgame_test/Test.h"

//-----------------------------------------------------------------------------

using Point = libboardgame_base::Point<19 * 19, 19, 19, unsigned short>;
using RectGeometry = libboardgame_base::RectGeometry<Point>;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgame_point_transform_get_transformed)
{
    unsigned sz = 9;
    auto& geo = RectGeometry::get(sz, sz);
    Point p = geo.get_point(1, 2);
    {
        libboardgame_base::PointTransfIdent<Point> transform;
        LIBBOARDGAME_CHECK(transform.get_transformed(p, geo) == p)
    }
    {
        libboardgame_base::PointTransfRot180<Point> transform;
        LIBBOARDGAME_CHECK(transform.get_transformed(p, geo)
                           == geo.get_point(7, 6))
    }
    {
        libboardgame_base::PointTransfRot270Refl<Point> transform;
        LIBBOARDGAME_CHECK(transform.get_transformed(p, geo)
                           == geo.get_point(2, 1))
    }
}

//-----------------------------------------------------------------------------
