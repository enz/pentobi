//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_sgf/UtilTest.cpp */
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
    unique_ptr<Node> root(new Node());
    auto& child = root->create_new_child();
    vector<const Node*> path;
    get_path_from_root(child, path);
    LIBBOARDGAME_CHECK_EQUAL(path.size(), 2u);
    LIBBOARDGAME_CHECK_EQUAL(path[0], root.get());
    LIBBOARDGAME_CHECK_EQUAL(path[1], &child);
}

//-----------------------------------------------------------------------------
