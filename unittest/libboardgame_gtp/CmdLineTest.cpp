//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_gtp/CmdLineTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_gtp/CmdLine.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_gtp;

//-----------------------------------------------------------------------------

namespace {

string get_id(const CmdLine& c)
{
    ostringstream s;
    c.write_id(s);
    return s.str();
}

string get_element(const CmdLine& c, unsigned i)
{
    return string(c.get_element(i));
}

} // namespace

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(gtp_cmd_line_init)
{
    CmdLine c("100 command1 arg1 arg2");
    LIBBOARDGAME_CHECK_EQUAL("100", get_id(c));
    LIBBOARDGAME_CHECK_EQUAL("command1", string(c.get_name()));
    LIBBOARDGAME_CHECK_EQUAL(4u, c.get_elements().size());
    LIBBOARDGAME_CHECK_EQUAL("arg1", get_element(c, 2));
    LIBBOARDGAME_CHECK_EQUAL("arg2", get_element(c, 3));
    c.init("2 command2 arg3");
    LIBBOARDGAME_CHECK_EQUAL("2", get_id(c));
    LIBBOARDGAME_CHECK_EQUAL("command2", string(c.get_name()));
    LIBBOARDGAME_CHECK_EQUAL(3u, c.get_elements().size());
    LIBBOARDGAME_CHECK_EQUAL("arg3", get_element(c, 2));
}

LIBBOARDGAME_TEST_CASE(gtp_cmd_line_parse)
{
    CmdLine c("10 boardsize 11");
    LIBBOARDGAME_CHECK_EQUAL("10 boardsize 11", c.get_line());
    LIBBOARDGAME_CHECK_EQUAL("11", string(c.get_trimmed_line_after_elem(1)));
    LIBBOARDGAME_CHECK_EQUAL("10", get_id(c));
    LIBBOARDGAME_CHECK_EQUAL("boardsize", string(c.get_name()));
    LIBBOARDGAME_CHECK_EQUAL(3u, c.get_elements().size());
    LIBBOARDGAME_CHECK_EQUAL("11", get_element(c, 2));

    c.init("  20 clear_board ");
    LIBBOARDGAME_CHECK_EQUAL("  20 clear_board ", c.get_line());
    LIBBOARDGAME_CHECK_EQUAL("", string(c.get_trimmed_line_after_elem(1)));
    LIBBOARDGAME_CHECK_EQUAL("20", get_id(c));
    LIBBOARDGAME_CHECK_EQUAL("clear_board", string(c.get_name()));
    LIBBOARDGAME_CHECK_EQUAL(2u, c.get_elements().size());
}

//-----------------------------------------------------------------------------
