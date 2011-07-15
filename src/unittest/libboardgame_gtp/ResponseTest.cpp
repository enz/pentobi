//-----------------------------------------------------------------------------
/** @file ResponseTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_gtp/Response.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_gtp;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(gtp_response_basic)
{
    Response r;
    r << "Name";
    LIBBOARDGAME_CHECK_EQUAL(string("Name"), r.to_string());
    r.set("Name2");
    LIBBOARDGAME_CHECK_EQUAL(string("Name2"), r.to_string());
}

//-----------------------------------------------------------------------------
