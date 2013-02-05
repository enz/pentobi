//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_mcts/Node.h"

#include <cmath>
#include <cstring>
#include "libboardgame_test/Test.h"

using namespace std;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(mcts_node_add_value)
{
    libboardgame_mcts::Node<int> node;
    node.init(0, 0.5, 0);
    node.add_value(5);
    LIBBOARDGAME_CHECK_CLOSE(node.get_value(), 5., 1e-4);
    node.add_value(2);
    LIBBOARDGAME_CHECK_CLOSE(node.get_value(), 3.5, 1e-4);
}

//-----------------------------------------------------------------------------
