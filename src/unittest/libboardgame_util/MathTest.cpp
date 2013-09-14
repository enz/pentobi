//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_util/MathTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_util/Math.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_util;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(util_math_cdf)
{
    double epsilon = 1e-6;
    LIBBOARDGAME_CHECK_CLOSE_EPS(cdf(-3.), 0.00134989803163, epsilon);
    LIBBOARDGAME_CHECK_CLOSE_EPS(cdf(-1.), 0.158655253931, epsilon);
    LIBBOARDGAME_CHECK_CLOSE_EPS(cdf(0.), 0.5, epsilon);
    LIBBOARDGAME_CHECK_CLOSE_EPS(cdf(0.5), 0.691462461274, epsilon);
    LIBBOARDGAME_CHECK_CLOSE_EPS(cdf(2.1), 0.982135579437, epsilon);
}

//-----------------------------------------------------------------------------
