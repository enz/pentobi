//-----------------------------------------------------------------------------
/** @file libboardgame_base/tests/RectGeometryTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_base/RectGeometry.h"

#include "libboardgame_base/Point.h"
#include "libboardgame_test/Test.h"

using namespace std;

//-----------------------------------------------------------------------------

using Point = libboardgame_base::Point<19 * 19, 19, 19, unsigned short>;
using Geometry = libboardgame_base::Geometry<Point>;
using RectGeometry = libboardgame_base::RectGeometry<Point>;
using PointList = libboardgame_base::ArrayList<Point, Point::range_onboard>;

//-----------------------------------------------------------------------------

namespace {

bool from_string(const string& s, const Geometry& geo, Point& p)
{
    return geo.from_string(s.begin(), s.end(), p);
}

} // namespace

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgame_rect_geometry_iterate)
{
    auto& geo = RectGeometry::get(3, 3);
    auto i = geo.begin();
    auto end = geo.end();
    LIBBOARDGAME_CHECK(i != end)
    LIBBOARDGAME_CHECK(geo.get_point(0, 0) == *i)
    ++i;
    LIBBOARDGAME_CHECK(i != end)
    LIBBOARDGAME_CHECK(geo.get_point(1, 0) == *i)
    ++i;
    LIBBOARDGAME_CHECK(i != end)
    LIBBOARDGAME_CHECK(geo.get_point(2, 0) == *i)
    ++i;
    LIBBOARDGAME_CHECK(i != end)
    LIBBOARDGAME_CHECK(geo.get_point(0, 1) == *i)
    ++i;
    LIBBOARDGAME_CHECK(i != end)
    LIBBOARDGAME_CHECK(geo.get_point(1, 1) == *i)
    ++i;
    LIBBOARDGAME_CHECK(i != end)
    LIBBOARDGAME_CHECK(geo.get_point(2, 1) == *i)
    ++i;
    LIBBOARDGAME_CHECK(i != end)
    LIBBOARDGAME_CHECK(geo.get_point(0, 2) == *i)
    ++i;
    LIBBOARDGAME_CHECK(i != end)
    LIBBOARDGAME_CHECK(geo.get_point(1, 2) == *i)
    ++i;
    LIBBOARDGAME_CHECK(i != end)
    LIBBOARDGAME_CHECK(geo.get_point(2, 2) == *i)
    ++i;
    LIBBOARDGAME_CHECK(i == end)
}

LIBBOARDGAME_TEST_CASE(boardgame_rect_geometry_from_string)
{
    auto& geo = RectGeometry::get(19, 19);
    Point p;

    LIBBOARDGAME_CHECK(from_string("a1", geo, p))
    LIBBOARDGAME_CHECK(p == geo.get_point(0, 18))

    LIBBOARDGAME_CHECK(from_string("a19", geo, p))
    LIBBOARDGAME_CHECK(p == geo.get_point(0, 0))

    LIBBOARDGAME_CHECK(from_string("A1", geo, p))
    LIBBOARDGAME_CHECK(p == geo.get_point(0, 18))

    LIBBOARDGAME_CHECK(! from_string("foobar", geo, p))
    LIBBOARDGAME_CHECK(! from_string("a123", geo, p))
    LIBBOARDGAME_CHECK(! from_string("a56", geo, p))
    LIBBOARDGAME_CHECK(! from_string("aa1", geo, p))
    LIBBOARDGAME_CHECK(! from_string("c3#", geo, p))
}

LIBBOARDGAME_TEST_CASE(boardgame_rect_geometry_to_string)
{
    auto& geo = RectGeometry::get(19, 19);
    LIBBOARDGAME_CHECK_EQUAL(string("a1"), geo.to_string(geo.get_point(0, 18)))
    LIBBOARDGAME_CHECK_EQUAL(string("a19"), geo.to_string(geo.get_point(0, 0)))
    LIBBOARDGAME_CHECK_EQUAL(string("j10"), geo.to_string(geo.get_point(9, 9)))
}

//-----------------------------------------------------------------------------
