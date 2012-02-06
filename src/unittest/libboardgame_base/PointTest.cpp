//-----------------------------------------------------------------------------
/** @file PointTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sstream>
#include "libboardgame_base/SpreadsheetStringRep.h"
#include "libboardgame_base/Point.h"
#include "libboardgame_test/Test.h"

using namespace std;
using libboardgame_base::SpreadsheetStringRep;

//-----------------------------------------------------------------------------

typedef libboardgame_base::Point<19,19,unsigned short,SpreadsheetStringRep>
    Point;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgame_point_from_string)
{
    LIBBOARDGAME_CHECK_EQUAL(Point(0, 0), Point::from_string("a1"));
    LIBBOARDGAME_CHECK_EQUAL(Point(0, 18), Point::from_string("a19"));
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
    LIBBOARDGAME_CHECK(! Point::null().is_onboard());
    LIBBOARDGAME_CHECK(Point(0, 0).is_onboard());
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
    LIBBOARDGAME_CHECK_EQUAL(string("j10"), Point(9, 9).to_string());
}

//-----------------------------------------------------------------------------
