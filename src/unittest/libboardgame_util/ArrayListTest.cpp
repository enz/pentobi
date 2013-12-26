//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_util/ArrayListTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sstream>
#include "libboardgame_util/ArrayList.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_util;

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

LIBBOARDGAME_TEST_CASE(util_array_list_construct_single_element)
{
    ArrayList<int, 10> l(5);
    LIBBOARDGAME_CHECK_EQUAL(1u, l.size());
    LIBBOARDGAME_CHECK_EQUAL(5, l[0]);
}

LIBBOARDGAME_TEST_CASE(util_array_list_equals)
{
    ArrayList<int, 10> l1;
    l1.push_back(1);
    l1.push_back(2);
    l1.push_back(3);
    ArrayList<int, 10> l2;
    l2.push_back(1);
    l2.push_back(2);
    l2.push_back(3);
    LIBBOARDGAME_CHECK(l1 == l2);
    l2.push_back(4);
    LIBBOARDGAME_CHECK(! (l1 == l2));
    l2.clear();
    l2.push_back(2);
    l2.push_back(1);
    l2.push_back(3);
    LIBBOARDGAME_CHECK(! (l1 == l2));
}

LIBBOARDGAME_TEST_CASE(util_array_list_pop_back)
{
    ArrayList<int, 10> l(5);
    int i = l.pop_back();
    LIBBOARDGAME_CHECK_EQUAL(5, i);
    LIBBOARDGAME_CHECK(l.empty());
}

LIBBOARDGAME_TEST_CASE(util_array_list_remove)
{
    ArrayList<int, 10> l;
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);
    l.push_back(4);
    l.remove(2);
    LIBBOARDGAME_CHECK_EQUAL(3u, l.size());
    LIBBOARDGAME_CHECK_EQUAL(1, l[0]);
    LIBBOARDGAME_CHECK_EQUAL(3, l[1]);
    LIBBOARDGAME_CHECK_EQUAL(4, l[2]);
}

//-----------------------------------------------------------------------------
