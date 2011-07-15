//-----------------------------------------------------------------------------
/** @file sgf/NodeTest.cpp */
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
    unique_ptr<Node> parent(Node::create());
    Node& child = parent->create_new_child();
    LIBBOARDGAME_CHECK_EQUAL(&parent->get_child(), &child);
    LIBBOARDGAME_CHECK_EQUAL(&child.get_parent(), parent.get());
}

//-----------------------------------------------------------------------------
