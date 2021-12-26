//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/tests/ResponseTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_gtp/Response.h"

#include "libboardgame_test/Test.h"

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
