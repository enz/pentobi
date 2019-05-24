//-----------------------------------------------------------------------------
/** @file libboardgame_base/tests/SgfUtilTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_base/SgfUtil.h"

#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_base;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(sgf_util_get_path_from_root)
{
    auto root = make_unique<SgfNode>();
    auto& child = root->create_new_child();
    vector<const SgfNode*> path;
    get_path_from_root(child, path);
    LIBBOARDGAME_CHECK_EQUAL(path.size(), 2u);
    LIBBOARDGAME_CHECK_EQUAL(path[0], root.get());
    LIBBOARDGAME_CHECK_EQUAL(path[1], &child);
}

//-----------------------------------------------------------------------------
