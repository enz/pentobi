//-----------------------------------------------------------------------------
/** @file NullTermListTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_test/Test.h"
#include "libboardgame_util/NullTermList.h"

using namespace std;
using namespace libboardgame_util;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(util_null_term_list_empty)
{
    NullTermList<int, 4> list;
    {
        NullTermList<int, 4>::Init init(list);
        init.finish();
    }
    LIBBOARDGAME_CHECK(list.empty());
    {
        NullTermList<int, 4>::Init init(list);
        init.push_back(5);
        init.finish();
    }
    LIBBOARDGAME_CHECK(! list.empty());
}

//-----------------------------------------------------------------------------
