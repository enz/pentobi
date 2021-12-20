//-----------------------------------------------------------------------------
/** @file libboardgame_base/tests/StatisticsTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_base/Statistics.h"
#include "libboardgame_test/Test.h"

using namespace libboardgame_base;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(libboardgame_base_statistics_basic)
{
    Statistics<double> s;
    s.add(12);
    s.add(11);
    s.add(14);
    s.add(16);
    s.add(15);
    LIBBOARDGAME_CHECK_EQUAL(s.get_count(), 5.);
    LIBBOARDGAME_CHECK_CLOSE_EPS(s.get_mean(), 13.6, 1e-6);
    LIBBOARDGAME_CHECK_CLOSE_EPS(s.get_variance(), 3.44, 1e-6);
    LIBBOARDGAME_CHECK_CLOSE_EPS(s.get_deviation(), 1.854723, 1e-6);
}

//-----------------------------------------------------------------------------
