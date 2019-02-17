//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_base/StringUtilTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_base/StringUtil.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_base;

//----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(libboardgame_base_get_letter_coord)
{
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(0), "a");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(1), "b");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(25), "z");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(26), "aa");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(26 + 1), "ab");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(26 + 25), "az");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(2 * 26), "ba");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(2 * 26 + 1), "bb");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(2 * 26 + 25), "bz");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(26 * 26), "za");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(26 * 26 + 1), "zb");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(26 * 26 + 25), "zz");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(27 * 26), "aaa");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(27 * 26 + 1), "aab");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(27 * 26 + 25), "aaz");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(28 * 26), "aba");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(28 * 26 + 1), "abb");
    LIBBOARDGAME_CHECK_EQUAL(get_letter_coord(28 * 26 + 25), "abz");
}

LIBBOARDGAME_TEST_CASE(libboardgame_base_split)
{
    {
        vector<string> v = split("a,b,cc,d", ',');
        LIBBOARDGAME_CHECK_EQUAL(v.size(), 4u);
        LIBBOARDGAME_CHECK_EQUAL(v[0], "a");
        LIBBOARDGAME_CHECK_EQUAL(v[1], "b");
        LIBBOARDGAME_CHECK_EQUAL(v[2], "cc");
        LIBBOARDGAME_CHECK_EQUAL(v[3], "d");
    }
    {
        vector<string> v = split("", ',');
        LIBBOARDGAME_CHECK_EQUAL(v.size(), 0u);
    }
    {
        vector<string> v = split("a,", ',');
        LIBBOARDGAME_CHECK_EQUAL(v.size(), 2u);
        LIBBOARDGAME_CHECK_EQUAL(v[0], "a");
        LIBBOARDGAME_CHECK_EQUAL(v[1], "");
    }
    {
        vector<string> v = split(",a", ',');
        LIBBOARDGAME_CHECK_EQUAL(v.size(), 2u);
        LIBBOARDGAME_CHECK_EQUAL(v[0], "");
        LIBBOARDGAME_CHECK_EQUAL(v[1], "a");
    }
    {
        vector<string> v = split("a,,b", ',');
        LIBBOARDGAME_CHECK_EQUAL(v.size(), 3u);
        LIBBOARDGAME_CHECK_EQUAL(v[0], "a");
        LIBBOARDGAME_CHECK_EQUAL(v[1], "");
        LIBBOARDGAME_CHECK_EQUAL(v[2], "b");
    }
}

LIBBOARDGAME_TEST_CASE(libboardgame_base_to_lower)
{
    LIBBOARDGAME_CHECK_EQUAL(to_lower("AabC "), "aabc ");
}

LIBBOARDGAME_TEST_CASE(libboardgame_base_trim)
{
    LIBBOARDGAME_CHECK_EQUAL(trim("aa bb"), "aa bb");
    LIBBOARDGAME_CHECK_EQUAL(trim(" \t\r\naa bb"), "aa bb");
    LIBBOARDGAME_CHECK_EQUAL(trim("aa bb \t\r\n"), "aa bb");
    LIBBOARDGAME_CHECK_EQUAL(trim(""), "");
}

//----------------------------------------------------------------------------
