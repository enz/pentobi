//-----------------------------------------------------------------------------
/** @file SpShtStrRepTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sstream>
#include "libboardgame_base/SpShtStrRep.h"
#include "libboardgame_test/Test.h"

using namespace std;
using libboardgame_base::SpShtStrRep;

//-----------------------------------------------------------------------------

namespace {

bool read(const string& s, unsigned& x, unsigned& y,
          unsigned width = SpShtStrRep::max_width,
          unsigned height = SpShtStrRep::max_height)
{
    istringstream in(s);
    return SpShtStrRep::read(in, width, height, x, y);
}

string write(unsigned x, unsigned y)
{
    ostringstream out;
    SpShtStrRep::write(out, x, y);
    return out.str();
}

} // namespace

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgame_base_spreadsheet_string_rep_read)
{
    unsigned x;
    unsigned y;

    LIBBOARDGAME_CHECK(read("a1", x, y));
    LIBBOARDGAME_CHECK_EQUAL(x, 0u);
    LIBBOARDGAME_CHECK_EQUAL(y, 0u);

    LIBBOARDGAME_CHECK(read("a23", x, y));
    LIBBOARDGAME_CHECK_EQUAL(x, 0u);
    LIBBOARDGAME_CHECK_EQUAL(y, 22u);

    LIBBOARDGAME_CHECK(read("A1", x, y));
    LIBBOARDGAME_CHECK_EQUAL(x, 0u);
    LIBBOARDGAME_CHECK_EQUAL(y, 0u);

    LIBBOARDGAME_CHECK(read("j1", x, y));
    LIBBOARDGAME_CHECK_EQUAL(x, 9u);
    LIBBOARDGAME_CHECK_EQUAL(y, 0u);

    LIBBOARDGAME_CHECK(read("ab1", x, y));
    LIBBOARDGAME_CHECK_EQUAL(x, 27u);
    LIBBOARDGAME_CHECK_EQUAL(y, 0u);

    LIBBOARDGAME_CHECK(read("  a1", x, y));
    LIBBOARDGAME_CHECK_EQUAL(x, 0u);
    LIBBOARDGAME_CHECK_EQUAL(y, 0u);

    LIBBOARDGAME_CHECK(! read("a 1", x, y));

    LIBBOARDGAME_CHECK(! read("foobar", x, y));

    LIBBOARDGAME_CHECK(! read("c3#", x, y));
}

LIBBOARDGAME_TEST_CASE(boardgame_base_spreadsheet_string_rep_write)
{
    LIBBOARDGAME_CHECK_EQUAL(string("a1"), write(0, 0));
    LIBBOARDGAME_CHECK_EQUAL(string("ab1"), write(27, 0));
    LIBBOARDGAME_CHECK_EQUAL(string("ba1"), write(52, 0));
}

//-----------------------------------------------------------------------------
