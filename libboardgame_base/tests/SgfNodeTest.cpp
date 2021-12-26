//-----------------------------------------------------------------------------
/** @file libboardgame_base/tests/SgfNodeTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_base/SgfNode.h"

#include <memory>
#include "libboardgame_test/Test.h"

using namespace libboardgame_base;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(sgf_node_create_new_child)
{
    auto parent = make_unique<SgfNode>();
    auto& child = parent->create_new_child();
    LIBBOARDGAME_CHECK_EQUAL(&parent->get_child(), &child);
    LIBBOARDGAME_CHECK_EQUAL(&child.get_parent(), parent.get());
}

LIBBOARDGAME_TEST_CASE(sgf_node_remove_property)
{
    string id = "B";
    auto node = make_unique<SgfNode>();
    LIBBOARDGAME_CHECK(! node->has_property(id));
    node->set_property(id, "foo");
    LIBBOARDGAME_CHECK(node->has_property(id));
    LIBBOARDGAME_CHECK_EQUAL(node->get_property(id), "foo");
    bool result = node->remove_property(id);
    LIBBOARDGAME_CHECK(result);
    LIBBOARDGAME_CHECK(! node->has_property(id));
}

//-----------------------------------------------------------------------------
