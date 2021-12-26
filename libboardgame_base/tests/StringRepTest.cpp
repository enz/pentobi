//-----------------------------------------------------------------------------
/** @file libboardgame_base/tests/StringRepTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_base/StringRep.h"

#include "libboardgame_test/Test.h"

using namespace std;
using libboardgame_base::StdStringRep;

//-----------------------------------------------------------------------------

namespace {

StdStringRep string_rep;

bool read(const string& s, unsigned& x, unsigned& y, unsigned width,
          unsigned height)
{
    return string_rep.read(s.begin(), s.end(), width, height, x, y);
}

string write(unsigned x, unsigned y, unsigned width, unsigned height)
{
    ostringstream out;
    string_rep.write(out, x, y, width, height);
    return out.str();
}

} // namespace

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgame_base_spreadsheet_string_rep_read)
{
    unsigned x;
    unsigned y;

    LIBBOARDGAME_CHECK(read("a1", x, y, 20, 20));
    LIBBOARDGAME_CHECK_EQUAL(x, 0u);
    LIBBOARDGAME_CHECK_EQUAL(y, 19u);

    LIBBOARDGAME_CHECK(read("a23", x, y, 25, 25));
    LIBBOARDGAME_CHECK_EQUAL(x, 0u);
    LIBBOARDGAME_CHECK_EQUAL(y, 2u);

    LIBBOARDGAME_CHECK(read("A1", x, y, 20, 20));
    LIBBOARDGAME_CHECK_EQUAL(x, 0u);
    LIBBOARDGAME_CHECK_EQUAL(y, 19u);

    LIBBOARDGAME_CHECK(read("j1", x, y, 20, 20));
    LIBBOARDGAME_CHECK_EQUAL(x, 9u);
    LIBBOARDGAME_CHECK_EQUAL(y, 19u);

    LIBBOARDGAME_CHECK(read("ab1", x, y, 30, 30));
    LIBBOARDGAME_CHECK_EQUAL(x, 27u);
    LIBBOARDGAME_CHECK_EQUAL(y, 29u);

    LIBBOARDGAME_CHECK(read("  a1", x, y, 20, 20));
    LIBBOARDGAME_CHECK_EQUAL(x, 0u);
    LIBBOARDGAME_CHECK_EQUAL(y, 19u);

    LIBBOARDGAME_CHECK(! read("a 1", x, y, 20, 20));

    LIBBOARDGAME_CHECK(! read("foobar", x, y, 20, 20));

    LIBBOARDGAME_CHECK(! read("c3#", x, y, 20, 20));
}

LIBBOARDGAME_TEST_CASE(boardgame_base_spreadsheet_string_rep_write)
{
    LIBBOARDGAME_CHECK_EQUAL(string("a1"), write(0, 18, 19, 19));
    LIBBOARDGAME_CHECK_EQUAL(string("a19"), write(0, 0, 19, 19));
    LIBBOARDGAME_CHECK_EQUAL(string("ab1"), write(27, 59, 60, 60));
    LIBBOARDGAME_CHECK_EQUAL(string("ba1"), write(52, 59, 60, 60));
}

//-----------------------------------------------------------------------------
