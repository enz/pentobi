//-----------------------------------------------------------------------------
/** @file NodeUtil.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_NODE_UTIL_H
#define LIBBOARDGAME_MCTS_NODE_UTIL_H

#include "ChildIterator.h"
#include "Node.h"

namespace libboardgame_mcts {
namespace node_util {

//-----------------------------------------------------------------------------

template<typename M>
const Node<M>* find_child(const Node<M>& node, M mv)
{
    for (ChildIterator<M> i(node); i; ++i)
        if (i->get_move() == mv)
            return &(*i);
    return 0;
}

//-----------------------------------------------------------------------------

} // namespace node_util
} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_NODE_UTIL_H
