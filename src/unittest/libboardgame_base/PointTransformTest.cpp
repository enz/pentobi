//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_base/PointTransformTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_base/PointTransform.h"
#include "libboardgame_base/SpShtStrRep.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_base;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgame_point_transform_get_transformed)
{
    typedef libboardgame_base::Point<19 * 19, unsigned short, SpShtStrRep>
        Point;

    unsigned sz = 9;
    Point p("B7", sz, sz);
    {
        PointTransfIdent<Point> transform;
        LIBBOARDGAME_CHECK(transform.get_transformed(p, sz, sz) == p);
    }
    {
        PointTransfRot180<Point> transform;
        LIBBOARDGAME_CHECK(transform.get_transformed(p, sz, sz)
                           == Point("H3", sz, sz));
    }
    {
        PointTransfRot270Refl<Point> transform;
        LIBBOARDGAME_CHECK(transform.get_transformed(p, sz, sz)
                           == Point("C8", sz, sz));
    }
}

//-----------------------------------------------------------------------------
