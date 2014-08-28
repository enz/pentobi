//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_base/PointTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sstream>
#include "libboardgame_base/SpShtStrRep.h"
#include "libboardgame_base/Point.h"
#include "libboardgame_test/Test.h"

using namespace std;
using libboardgame_base::SpShtStrRep;

//-----------------------------------------------------------------------------

typedef libboardgame_base::Point<19 * 19, unsigned short, SpShtStrRep> Point;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgame_point_from_string)
{
    LIBBOARDGAME_CHECK(Point(0, 18, 19) == Point::from_string("a1", 19, 19));
    LIBBOARDGAME_CHECK(Point(0, 0, 19) == Point::from_string("a19", 19, 19));
    LIBBOARDGAME_CHECK(Point(0, 18, 19) == Point::from_string("A1", 19, 19));
    LIBBOARDGAME_CHECK(Point(0, 18, 19) == Point::from_string("  A1", 19, 19));
    LIBBOARDGAME_CHECK(
                Point(0, 18, 19) == Point::from_string("a1 \t", 19, 19));
    LIBBOARDGAME_CHECK_THROW(Point::from_string("a 19", 19, 19),
                             Point::InvalidString);
    LIBBOARDGAME_CHECK_THROW(Point::from_string("a1 foo", 19, 19),
                             Point::InvalidString);
    LIBBOARDGAME_CHECK_THROW(Point::from_string("a19 foo", 19, 19),
                             Point::InvalidString);
    LIBBOARDGAME_CHECK_THROW(Point::from_string("NULL foo", 19, 19),
                             Point::InvalidString);
    LIBBOARDGAME_CHECK_THROW(Point::from_string("foobar", 19, 19),
                             Point::InvalidString);
    LIBBOARDGAME_CHECK_THROW(Point::from_string("a123", 19, 19),
                             Point::InvalidString);
    LIBBOARDGAME_CHECK_THROW(Point::from_string("a56", 19, 19),
                             Point::InvalidString);
    LIBBOARDGAME_CHECK_THROW(Point::from_string("aa1", 19, 19),
                             Point::InvalidString);
    LIBBOARDGAME_CHECK_THROW(Point::from_string("c3#", 19, 19),
                             Point::InvalidString);
}

LIBBOARDGAME_TEST_CASE(boardgame_point_get_down)
{
    LIBBOARDGAME_CHECK(Point(0, 18, 19) == Point(0, 17, 19).get_down(19));
}

LIBBOARDGAME_TEST_CASE(boardgame_point_get_left)
{
    LIBBOARDGAME_CHECK(Point(0, 0, 19) == Point(1, 0, 19).get_left());
}

LIBBOARDGAME_TEST_CASE(boardgame_point_get_right)
{
    LIBBOARDGAME_CHECK(Point(1, 0, 19) == Point(0, 0, 19).get_right());
}

LIBBOARDGAME_TEST_CASE(boardgame_point_get_up)
{
    LIBBOARDGAME_CHECK(Point(0, 0, 19) == Point(0, 1, 19).get_up(19));
}

LIBBOARDGAME_TEST_CASE(boardgame_point_is_onboard)
{
    LIBBOARDGAME_CHECK(! Point::null().is_onboard());
    LIBBOARDGAME_CHECK(Point(0, 0, 19).is_onboard());
}

LIBBOARDGAME_TEST_CASE(boardgame_point_to_string)
{
    LIBBOARDGAME_CHECK_EQUAL(string("a1"), Point(0, 18, 19).to_string(19, 19));
    LIBBOARDGAME_CHECK_EQUAL(string("a19"), Point(0, 0, 19).to_string(19, 19));
    LIBBOARDGAME_CHECK_EQUAL(string("j10"), Point(9, 9, 19).to_string(19, 19));
}

//-----------------------------------------------------------------------------
