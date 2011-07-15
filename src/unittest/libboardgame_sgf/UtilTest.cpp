//-----------------------------------------------------------------------------
/** @file sgf/UtilTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_sgf/Util.h"

#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_sgf;
using namespace libboardgame_sgf::util;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(sgf_util_get_path_from_root)
{
    unique_ptr<Node> root(Node::create());
    const Node& child = root->create_new_child();
    vector<const Node*> path = get_path_from_root(child);
    LIBBOARDGAME_CHECK_EQUAL(path.size(), 2u);
    LIBBOARDGAME_CHECK_EQUAL(path[0], root.get());
    LIBBOARDGAME_CHECK_EQUAL(path[1], &child);
}

LIBBOARDGAME_TEST_CASE(sgf_util_parse_go_move_property_value)
{
    int x;
    int y;

    parse_go_move_property_value("", x, y, 19);
    LIBBOARDGAME_CHECK_EQUAL(x, -1);
    LIBBOARDGAME_CHECK_EQUAL(y, -1);

    parse_go_move_property_value("tt", x, y, 19);
    LIBBOARDGAME_CHECK_EQUAL(x, -1);
    LIBBOARDGAME_CHECK_EQUAL(y, -1);

    parse_go_move_property_value("tt", x, y, 9);
    LIBBOARDGAME_CHECK_EQUAL(x, -1);
    LIBBOARDGAME_CHECK_EQUAL(y, -1);
}

LIBBOARDGAME_TEST_CASE(sgf_util_parse_go_point_property_value)
{
    int x;
    int y;

    parse_go_move_property_value("aa", x, y, 19);
    LIBBOARDGAME_CHECK_EQUAL(x, 0);
    LIBBOARDGAME_CHECK_EQUAL(y, 18);

    parse_go_move_property_value("aa", x, y, 9);
    LIBBOARDGAME_CHECK_EQUAL(x, 0);
    LIBBOARDGAME_CHECK_EQUAL(y, 8);

    parse_go_move_property_value("kk", x, y, 19);
    LIBBOARDGAME_CHECK_EQUAL(x, 10);
    LIBBOARDGAME_CHECK_EQUAL(y, 8);

    LIBBOARDGAME_CHECK_THROW(parse_go_move_property_value("AA", x, y, 19),
                             InvalidPropertyValue);

    parse_go_move_property_value("AA", x, y, 30);
    LIBBOARDGAME_CHECK_EQUAL(x, 27);
    LIBBOARDGAME_CHECK_EQUAL(y, 2);

    parse_go_move_property_value("tt", x, y, 20);
    LIBBOARDGAME_CHECK_EQUAL(x, 19);
    LIBBOARDGAME_CHECK_EQUAL(y, 0);

    LIBBOARDGAME_CHECK_THROW(parse_go_move_property_value("foo", x, y, 19),
                             InvalidPropertyValue);

    LIBBOARDGAME_CHECK_THROW(parse_go_move_property_value("f", x, y, 19),
                             InvalidPropertyValue);

    LIBBOARDGAME_CHECK_THROW(parse_go_move_property_value("rr", x, y, 9),
                             InvalidPropertyValue);
}

//-----------------------------------------------------------------------------
