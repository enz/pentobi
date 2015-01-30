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

typedef
libboardgame_base::Point<19 * 19, 19, 19, unsigned short, SpShtStrRep> Point;
typedef libboardgame_base::Geometry<Point> Geometry;
typedef libboardgame_base::RectGeometry<Point> RectGeometry;
typedef libboardgame_base::PointList<Point> PointList;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgame_rect_geometry_iterate)
{
    auto& geo = RectGeometry::get(3, 3);
    auto i = geo.begin();
    auto end = geo.end();
    LIBBOARDGAME_CHECK(i != end);
    LIBBOARDGAME_CHECK(geo.get_point(0, 0) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i != end);
    LIBBOARDGAME_CHECK(geo.get_point(1, 0) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i != end);
    LIBBOARDGAME_CHECK(geo.get_point(2, 0) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i != end);
    LIBBOARDGAME_CHECK(geo.get_point(0, 1) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i != end);
    LIBBOARDGAME_CHECK(geo.get_point(1, 1) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i != end);
    LIBBOARDGAME_CHECK(geo.get_point(2, 1) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i != end);
    LIBBOARDGAME_CHECK(geo.get_point(0, 2) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i != end);
    LIBBOARDGAME_CHECK(geo.get_point(1, 2) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i != end);
    LIBBOARDGAME_CHECK(geo.get_point(2, 2) == *i);
    ++i;
    LIBBOARDGAME_CHECK(i == end);
}

LIBBOARDGAME_TEST_CASE(boardgame_rect_geometry_from_string)
{
    auto& geo = RectGeometry::get(19, 19);
    Point p;

    LIBBOARDGAME_CHECK(geo.from_string("a1", p));
    LIBBOARDGAME_CHECK(p == geo.get_point(0, 18));

    LIBBOARDGAME_CHECK(geo.from_string("a19", p));
    LIBBOARDGAME_CHECK(p == geo.get_point(0, 0));

    LIBBOARDGAME_CHECK(geo.from_string("A1", p));
    LIBBOARDGAME_CHECK(p == geo.get_point(0, 18));

    LIBBOARDGAME_CHECK(! geo.from_string("foobar", p));
    LIBBOARDGAME_CHECK(! geo.from_string("a123", p));
    LIBBOARDGAME_CHECK(! geo.from_string("a56", p));
    LIBBOARDGAME_CHECK(! geo.from_string("aa1", p));
    LIBBOARDGAME_CHECK(! geo.from_string("c3#", p));
}

LIBBOARDGAME_TEST_CASE(boardgame_rect_geometry_to_string)
{
    auto& geo = RectGeometry::get(19, 19);
    LIBBOARDGAME_CHECK_EQUAL(string("a1"), geo.to_string(geo.get_point(0, 18)));
    LIBBOARDGAME_CHECK_EQUAL(string("a19"), geo.to_string(geo.get_point(0, 0)));
    LIBBOARDGAME_CHECK_EQUAL(string("j10"), geo.to_string(geo.get_point(9, 9)));
}

//-----------------------------------------------------------------------------
