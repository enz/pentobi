//-----------------------------------------------------------------------------
/** @file TransformTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_base/SpreadsheetStringRep.h"
#include "libboardgame_base/Transform.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_base;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgame_transform_get_transformed_point)
{
    typedef Point<19, 19, SpreadsheetStringRep> Point;
    unsigned int sz = 9;
    Point p("B7");
    {
        Transform transform(Transform::identity);
        LIBBOARDGAME_CHECK_EQUAL(transform.get_transformed(p, sz, sz), p);
    }
    {
        Transform transform(Transform::rotate_90);
        LIBBOARDGAME_CHECK_EQUAL(transform.get_transformed(p, sz, sz),
                                 Point("G8"));
    }
    {
        Transform transform(Transform::rotate_180);
        LIBBOARDGAME_CHECK_EQUAL(transform.get_transformed(p, sz, sz),
                                 Point("H3"));
    }
    {
        Transform transform(Transform::rotate_270);
        LIBBOARDGAME_CHECK_EQUAL(transform.get_transformed(p, sz, sz),
                                 Point("C2"));
    }
    {
        Transform transform(Transform::mirror);
        LIBBOARDGAME_CHECK_EQUAL(transform.get_transformed(p, sz, sz),
                                 Point("H7"));
    }
    {
        Transform transform(Transform::rotate_90_mirror);
        LIBBOARDGAME_CHECK_EQUAL(transform.get_transformed(p, sz, sz),
                                 Point("G2"));
    }
    {
        Transform transform(Transform::rotate_180_mirror);
        LIBBOARDGAME_CHECK_EQUAL(transform.get_transformed(p, sz, sz),
                                 Point("B3"));
    }
    {
        Transform transform(Transform::rotate_270_mirror);
        LIBBOARDGAME_CHECK_EQUAL(transform.get_transformed(p, sz, sz),
                                 Point("C8"));
    }
}

//-----------------------------------------------------------------------------
