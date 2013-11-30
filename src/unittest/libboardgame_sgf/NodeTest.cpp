//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_sgf/NodeTest.cpp
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <memory>
#include "libboardgame_sgf/Node.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_sgf;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(sgf_node_create_new_child)
{
    unique_ptr<Node> parent(new Node());
    auto& child = parent->create_new_child();
    LIBBOARDGAME_CHECK_EQUAL(&parent->get_child(), &child);
    LIBBOARDGAME_CHECK_EQUAL(&child.get_parent(), parent.get());
}

LIBBOARDGAME_TEST_CASE(sgf_node_remove_property)
{
    string id = "B";
    unique_ptr<Node> node(new Node());
    LIBBOARDGAME_CHECK(! node->has_property(id));
    node->set_property(id, "foo");
    LIBBOARDGAME_CHECK(node->has_property(id));
    LIBBOARDGAME_CHECK_EQUAL(node->get_property(id), "foo");
    bool result = node->remove_property(id);
    LIBBOARDGAME_CHECK(result);
    LIBBOARDGAME_CHECK(! node->has_property(id));
}

//-----------------------------------------------------------------------------
