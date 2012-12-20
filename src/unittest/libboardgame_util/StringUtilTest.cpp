//-----------------------------------------------------------------------------
/** @file StringUtilTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_util/StringUtil.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_util;

//----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(util_get_letter_coord)
{
    using string_util::get_letter_coord;
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

//----------------------------------------------------------------------------
