//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_util/FastLogTest.cpp
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cmath>
#include <iostream>
#include "libboardgame_util/FastLog.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_util;

//----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(util_fast_log_basic)
{
    FastLog fast_log(10);
    float epsilon = 0.1f;
    LIBBOARDGAME_CHECK_CLOSE(fast_log.get_log(1), 0.f, epsilon);
    LIBBOARDGAME_CHECK_CLOSE(fast_log.get_log(exp(1.f)), 1.f, epsilon);
    LIBBOARDGAME_CHECK_CLOSE(fast_log.get_log(pow(exp(1.f), 2.f)), 2.f,
                             epsilon);
}

//----------------------------------------------------------------------------
