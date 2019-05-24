//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/tests/NodeTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_mcts/Node.h"

#include "libboardgame_test/Test.h"

using namespace std;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(libboardgame_mcts_node_add_value)
{
    libboardgame_mcts::Node<int, float, true> node;
    node.init(0, 0.5, 0, 1);
    node.add_value(5);
    LIBBOARDGAME_CHECK_CLOSE(node.get_value(), 5.f, 1e-4f);
    node.add_value(2);
    LIBBOARDGAME_CHECK_CLOSE(node.get_value(), 3.5f, 1e-4f);
}

LIBBOARDGAME_TEST_CASE(libboardgame_mcts_node_add_value_remove_loss)
{
    libboardgame_mcts::Node<int, float, true> node;
    node.init(0, 0.5, 0, 1);
    node.add_value(5);
    LIBBOARDGAME_CHECK_CLOSE(node.get_value(), 5.f, 1e-4f);
    node.add_value(0);
    LIBBOARDGAME_CHECK_CLOSE(node.get_value(), 2.5f, 1e-4f);
    node.add_value_remove_loss(2);
    LIBBOARDGAME_CHECK_CLOSE(node.get_value(), 3.5f, 1e-4f);
}

//-----------------------------------------------------------------------------
