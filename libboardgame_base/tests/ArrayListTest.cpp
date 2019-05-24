//-----------------------------------------------------------------------------
/** @file libboardgame_base/tests/ArrayListTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_base/ArrayList.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_base;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(util_array_list_basic)
{
    ArrayList<int, 10> l;
    LIBBOARDGAME_CHECK_EQUAL(0u, l.size());
    LIBBOARDGAME_CHECK(l.empty());
    l.push_back(5);
    LIBBOARDGAME_CHECK_EQUAL(1u, l.size());
    LIBBOARDGAME_CHECK(! l.empty());
    LIBBOARDGAME_CHECK_EQUAL(5, l[0]);
    l.push_back(7);
    LIBBOARDGAME_CHECK_EQUAL(2u, l.size());
    LIBBOARDGAME_CHECK(! l.empty());
    LIBBOARDGAME_CHECK_EQUAL(5, l[0]);
    LIBBOARDGAME_CHECK_EQUAL(7, l[1]);
    l.clear();
    LIBBOARDGAME_CHECK_EQUAL(0u, l.size());
    LIBBOARDGAME_CHECK(l.empty());
}

LIBBOARDGAME_TEST_CASE(util_array_list_equals)
{
    ArrayList<int, 10> l1{ 1, 2, 3 };
    ArrayList<int, 10> l2{ 1, 2, 3 };
    LIBBOARDGAME_CHECK(l1 == l2);
    l2.push_back(4);
    LIBBOARDGAME_CHECK(! (l1 == l2));
    l2 = ArrayList<int, 10>({ 2, 1, 3 });
    LIBBOARDGAME_CHECK(! (l1 == l2));
}

LIBBOARDGAME_TEST_CASE(util_array_list_pop_back)
{
    ArrayList<int, 10> l({ 5 });
    int i = l.pop_back();
    LIBBOARDGAME_CHECK_EQUAL(5, i);
    LIBBOARDGAME_CHECK(l.empty());
}

LIBBOARDGAME_TEST_CASE(util_array_list_remove)
{
    ArrayList<int, 10> l{ 1, 2, 3, 4 };
    l.remove(2);
    LIBBOARDGAME_CHECK_EQUAL(3u, l.size());
    LIBBOARDGAME_CHECK_EQUAL(1, l[0]);
    LIBBOARDGAME_CHECK_EQUAL(3, l[1]);
    LIBBOARDGAME_CHECK_EQUAL(4, l[2]);
}

//-----------------------------------------------------------------------------
