//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_util/MathUtilTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_util/MathUtil.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_util;

//----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(libboardgame_util_rounded_log_2)
{
    LIBBOARDGAME_CHECK_EQUAL(rounded_log_2(1.f), 0);
    LIBBOARDGAME_CHECK_EQUAL(rounded_log_2(1.1f), 0);
    LIBBOARDGAME_CHECK_EQUAL(rounded_log_2(1.9f), 0);
    LIBBOARDGAME_CHECK_EQUAL(rounded_log_2(2.f), 1);
    LIBBOARDGAME_CHECK_EQUAL(rounded_log_2(4.f), 2);

    LIBBOARDGAME_CHECK_EQUAL(rounded_log_2(1.), 0);
    LIBBOARDGAME_CHECK_EQUAL(rounded_log_2(1.1), 0);
    LIBBOARDGAME_CHECK_EQUAL(rounded_log_2(1.9), 0);
    LIBBOARDGAME_CHECK_EQUAL(rounded_log_2(2.), 1);
    LIBBOARDGAME_CHECK_EQUAL(rounded_log_2(4.), 2);
}

//----------------------------------------------------------------------------
