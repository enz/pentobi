//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_base/RectGeometryTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_base/PointList.h"
#include "libboardgame_base/RectGeometry.h"
#include "libboardgame_base/SpShtStrRep.h"
#include "libboardgame_test/Test.h"

using namespace std;
using libboardgame_base::SpShtStrRep;

//-----------------------------------------------------------------------------

typedef libboardgame_base::Point<19 * 19, unsigned short, SpShtStrRep> Point;
typedef libboardgame_base::Geometry<Point> Geometry;
typedef libboardgame_base::RectGeometry<Point> RectGeometry;
typedef libboardgame_base::PointList<Point> PointList;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgame_rect_geometry_iterate)
{
    auto& geo = RectGeometry::get(3, 3);
    Geometry::Iterator i(geo);
    LIBBOARDGAME_CHECK(i);
    LIBBOARDGAME_CHECK(Point(0, 0, 3) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i);
    LIBBOARDGAME_CHECK(Point(1, 0, 3) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i);
    LIBBOARDGAME_CHECK(Point(2, 0, 3) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i);
    LIBBOARDGAME_CHECK(Point(0, 1, 3) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i);
    LIBBOARDGAME_CHECK(Point(1, 1, 3) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i);
    LIBBOARDGAME_CHECK(Point(2, 1, 3) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i);
    LIBBOARDGAME_CHECK(Point(0, 2, 3) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i);
    LIBBOARDGAME_CHECK(Point(1, 2, 3) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i);
    LIBBOARDGAME_CHECK(Point(2, 2, 3) == *i);
    ++i;
    LIBBOARDGAME_CHECK(! i);
}

//-----------------------------------------------------------------------------
