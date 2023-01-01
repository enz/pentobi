//-----------------------------------------------------------------------------
/** @file libboardgame_base/tests/MarkerTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_base/Marker.h"

#include "libboardgame_base/Point.h"
#include "libboardgame_test/Test.h"

using namespace std;

//-----------------------------------------------------------------------------

using Point = libboardgame_base::Point<19 * 19, 19, 19, unsigned short>;
using Marker = libboardgame_base::Marker<Point>;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgame_marker_basic)
{
    Marker m;
    Point p1(10);
    Point p2(11);
    LIBBOARDGAME_CHECK(! m.set(p1));
    LIBBOARDGAME_CHECK(! m.set(p2));
    LIBBOARDGAME_CHECK(m.set(p1));
    LIBBOARDGAME_CHECK(m.set(p2));
    m.clear();
    LIBBOARDGAME_CHECK(! m.set(p1));
    LIBBOARDGAME_CHECK(! m.set(p2));
}

/** Test clear after a number of clears around the maximum unsigned integer
    value.
    This is a critical point of the implementation, which assumes that
    values not equal to a clear counter are unmarked and the overflow of the
    clear counter must be handled correctly. */
LIBBOARDGAME_TEST_CASE(boardgame_marker_overflow)
{
    Marker m;
    m.setup_for_overflow_test(numeric_limits<unsigned>::max() - 5);
    Point p1(10);
    Point p2(11);
    for (int i = 0; i < 10; ++i)
    {
        LIBBOARDGAME_CHECK(! m.set(p1));
        LIBBOARDGAME_CHECK(! m.set(p2));
        m.clear();
    }
}

//-----------------------------------------------------------------------------
