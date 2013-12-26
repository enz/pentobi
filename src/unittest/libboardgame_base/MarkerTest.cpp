//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_base/MarkerTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_base/SpShtStrRep.h"
#include "libboardgame_base/Marker.h"
#include "libboardgame_base/Point.h"
#include "libboardgame_test/Test.h"

using namespace std;
using libboardgame_base::SpShtStrRep;

//-----------------------------------------------------------------------------

typedef libboardgame_base::Point<19, 19, unsigned short, SpShtStrRep> Point;
typedef libboardgame_base::Marker<Point> Marker;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgame_marker_basic)
{
    Marker m;
    Point p1(0, 1);
    Point p2(0, 0);
    LIBBOARDGAME_CHECK(! m[p1]);
    LIBBOARDGAME_CHECK(! m[p2]);
    m.set(p1);
    LIBBOARDGAME_CHECK(m[p1]);
    LIBBOARDGAME_CHECK(! m[p2]);
    m.clear();
    LIBBOARDGAME_CHECK(! m[p1]);
    LIBBOARDGAME_CHECK(! m[p2]);
}

/** Test clear after a number of clears around the maximum unsigned integer
    value.
    This is a critical point of the implementation, which assumes that
    values not equal to a clear counter are unmarked and the overflow of the
    clear counter must be handled correctly.
    This test is only run, if integers are not larger than 32-bit, otherwise
    it would take too long. */
LIBBOARDGAME_TEST_CASE(boardgame_marker_overflow)
{
    if (numeric_limits<unsigned>::digits > 32)
        return;
    Marker m;
    m.setup_for_overflow_test(numeric_limits<unsigned>::max() - 5);
    Point p1(0, 1);
    Point p2(0, 0);
    for (int i = 0; i < 10; ++i)
    {
        LIBBOARDGAME_CHECK(! m[p1]);
        LIBBOARDGAME_CHECK(! m[p2]);
        m.set(p1);
        LIBBOARDGAME_CHECK(m[p1]);
        LIBBOARDGAME_CHECK(! m[p2]);
        m.clear();
        LIBBOARDGAME_CHECK(! m[p1]);
        LIBBOARDGAME_CHECK(! m[p2]);
    }
}

//-----------------------------------------------------------------------------
