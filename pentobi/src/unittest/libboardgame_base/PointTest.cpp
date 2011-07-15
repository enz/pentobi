//-----------------------------------------------------------------------------
/** @file PointTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sstream>
#include "libboardgame_base/GoPointStringRep.h"
#include "libboardgame_base/Point.h"
#include "libboardgame_test/Test.h"

using namespace std;
using libboardgame_base::GoPointStringRep;

//-----------------------------------------------------------------------------

typedef libboardgame_base::Point<19, GoPointStringRep> Point;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgame_point_from_string)
{
    LIBBOARDGAME_CHECK_EQUAL(Point(0, 0), Point::from_string("a1"));
    LIBBOARDGAME_CHECK_EQUAL(Point(0, 18), Point::from_string("a19"));
    LIBBOARDGAME_CHECK_EQUAL(Point(18, 0), Point::from_string("t1"));
    LIBBOARDGAME_CHECK_EQUAL(Point(18, 18), Point::from_string("t19"));
    LIBBOARDGAME_CHECK_EQUAL(Point(8, 9), Point::from_string("j10"));
    LIBBOARDGAME_CHECK_EQUAL(Point(0, 0), Point::from_string("A1"));
    LIBBOARDGAME_CHECK_EQUAL(Point(0, 0), Point::from_string("  A1"));
    LIBBOARDGAME_CHECK_EQUAL(Point(0, 0), Point::from_string("a1 \t"));
    LIBBOARDGAME_CHECK_THROW(Point::from_string("a 19"), Point::InvalidString);
    LIBBOARDGAME_CHECK_THROW(Point::from_string("a1 foo"),
                             Point::InvalidString);
    LIBBOARDGAME_CHECK_THROW(Point::from_string("a19 foo"),
                             Point::InvalidString);
    LIBBOARDGAME_CHECK_THROW(Point::from_string("NULL foo"),
                             Point::InvalidString);
    LIBBOARDGAME_CHECK_THROW(Point::from_string("foobar"),
                             Point::InvalidString);
    LIBBOARDGAME_CHECK_THROW(Point::from_string("a123"), Point::InvalidString);
    LIBBOARDGAME_CHECK_THROW(Point::from_string("a56"), Point::InvalidString);
    LIBBOARDGAME_CHECK_THROW(Point::from_string("aa1"), Point::InvalidString);
    LIBBOARDGAME_CHECK_THROW(Point::from_string("c3#"), Point::InvalidString);
}

LIBBOARDGAME_TEST_CASE(boardgame_point_get_down)
{
    LIBBOARDGAME_CHECK_EQUAL(Point(0, 17), Point(0, 18).get_down());
}

LIBBOARDGAME_TEST_CASE(boardgame_point_get_left)
{
    LIBBOARDGAME_CHECK_EQUAL(Point(0, 0), Point(1, 0).get_left());
}

LIBBOARDGAME_TEST_CASE(boardgame_point_get_right)
{
    LIBBOARDGAME_CHECK_EQUAL(Point(1, 0), Point(0, 0).get_right());
}

LIBBOARDGAME_TEST_CASE(boardgame_point_get_up)
{
    LIBBOARDGAME_CHECK_EQUAL(Point(0, 1), Point(0, 0).get_up());
}

LIBBOARDGAME_TEST_CASE(boardgame_point_is_onboard)
{
    unsigned int max = Point::max_size - 1;
    LIBBOARDGAME_CHECK(Point(0, 0).is_onboard());
    LIBBOARDGAME_CHECK(! Point(0, 0).get_left().is_onboard());
    LIBBOARDGAME_CHECK(! Point(0, 0).get_down().is_onboard());
    LIBBOARDGAME_CHECK(Point(0, max).is_onboard());
    LIBBOARDGAME_CHECK(! Point(0, max).get_left().is_onboard());
    LIBBOARDGAME_CHECK(! Point(0, max).get_up().is_onboard());
    LIBBOARDGAME_CHECK(Point(max, 0).is_onboard());
    LIBBOARDGAME_CHECK(! Point(max, 0).get_down().is_onboard());
    LIBBOARDGAME_CHECK(! Point(max, 0).get_right().is_onboard());
    LIBBOARDGAME_CHECK(Point(max, max).is_onboard());
    LIBBOARDGAME_CHECK(! Point(max, max).get_up().is_onboard());
    LIBBOARDGAME_CHECK(! Point(max, max).get_right().is_onboard());
}

LIBBOARDGAME_TEST_CASE(boardgame_point_stream_output)
{
    ostringstream o;
    o << Point(0, 0);
    LIBBOARDGAME_CHECK_EQUAL(string("a1"), o.str());
}

LIBBOARDGAME_TEST_CASE(boardgame_point_to_string)
{
    LIBBOARDGAME_CHECK_EQUAL(string("a1"), Point(0, 0).to_string());
    LIBBOARDGAME_CHECK_EQUAL(string("a19"), Point(0, 18).to_string());
    LIBBOARDGAME_CHECK_EQUAL(string("t1"), Point(18, 0).to_string());
    LIBBOARDGAME_CHECK_EQUAL(string("t19"), Point(18, 18).to_string());
    LIBBOARDGAME_CHECK_EQUAL(string("j10"), Point(8, 9).to_string());
}

//-----------------------------------------------------------------------------
