//-----------------------------------------------------------------------------
/** @file PointTransformTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_base/PointTransform.h"
#include "libboardgame_base/SpreadsheetStringRep.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_base;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgame_point_transform_get_transformed)
{
    typedef
      libboardgame_base::Point<19, 19, short unsigned int, SpreadsheetStringRep>
      Point;
    unsigned int sz = 9;
    Point p("B7");
    {
        PointTransfIdent<Point> transform;
        LIBBOARDGAME_CHECK_EQUAL(transform.get_transformed(p, sz, sz), p);
    }
    {
        PointTransfRot180<Point> transform;
        LIBBOARDGAME_CHECK_EQUAL(transform.get_transformed(p, sz, sz),
                                 Point("H3"));
    }
    {
        PointTransfRot270Refl<Point> transform;
        LIBBOARDGAME_CHECK_EQUAL(transform.get_transformed(p, sz, sz),
                                 Point("C8"));
    }
}

//-----------------------------------------------------------------------------
