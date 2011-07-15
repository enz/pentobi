//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_mcts/Node.h"

#include <cstring>
#include "libboardgame_test/Test.h"

using namespace std;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(mcts_node_add_value)
{
    typedef libboardgame_mcts::Node<int> Node;
    // Construct node in a memory filled with 0xff to avoid that it is
    // accidentally filled with 0, even if the constructor does not initialize
    // Node::m_value (m_value either needs to be initialized with 0, or the
    // incremental algorithm in add_value() needs to handle the case
    // m_count == 0 separately)
    char memory[sizeof(Node)];
    memset(memory, 0xff, sizeof(Node));
    Node* node = new(memory) Node;
    node->clear();
    node->add_value(5);
    LIBBOARDGAME_CHECK_CLOSE(node->get_value(), 5., 1e-4);
    node->add_value(2);
    LIBBOARDGAME_CHECK_CLOSE(node->get_value(), 3.5, 1e-4);
}

//-----------------------------------------------------------------------------
