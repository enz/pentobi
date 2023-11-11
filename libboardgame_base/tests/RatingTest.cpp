//-----------------------------------------------------------------------------
/** @file libboardgame_base/tests/RatingTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_base/Rating.h"

#include "libboardgame_test/Test.h"

using namespace libboardgame_base;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgame_rating_get_expected_result)
{
    Rating a(2806);
    Rating b(2577);
    LIBBOARDGAME_CHECK_CLOSE_EPS(a.get_expected_result(b), 0.789, 0.001)
}

LIBBOARDGAME_TEST_CASE(boardgame_rating_get_expected_result_multiplayer)
{
    // Player and 3 opponents, all with rating 1000, should have 25%
    // winning probability
    Rating a(1000);
    Rating b(1000);
    LIBBOARDGAME_CHECK_CLOSE_EPS(a.get_expected_result(b, 3), 0.25, 0.001)
}

LIBBOARDGAME_TEST_CASE(boardgame_rating_update_1)
{
    Rating a(2806);
    Rating b(2577);
    Rating new_a = a;
    Rating new_b = b;
    new_a.update(0, b, 10);
    new_b.update(1, a, 10);
    LIBBOARDGAME_CHECK_CLOSE_EPS(new_a.get(), 2798, 1)
    LIBBOARDGAME_CHECK_CLOSE_EPS(new_b.get(), 2585, 1)
}

LIBBOARDGAME_TEST_CASE(boardgame_rating_update_2)
{
    Rating a(2806);
    Rating b(2577);
    Rating new_a = a;
    Rating new_b = b;
    new_a.update(1, b, 10);
    new_b.update(0, a, 10);
    LIBBOARDGAME_CHECK_CLOSE_EPS(new_a.get(), 2808, 1)
    LIBBOARDGAME_CHECK_CLOSE_EPS(new_b.get(), 2575, 1)
}

LIBBOARDGAME_TEST_CASE(boardgame_rating_update_3)
{
    Rating a(2806);
    Rating b(2577);
    Rating new_a = a;
    Rating new_b = b;
    new_a.update(0.5, b, 10);
    new_b.update(0.5, a, 10);
    LIBBOARDGAME_CHECK_CLOSE_EPS(new_a.get(), 2803, 1)
    LIBBOARDGAME_CHECK_CLOSE_EPS(new_b.get(), 2580, 1)
}

//-----------------------------------------------------------------------------
