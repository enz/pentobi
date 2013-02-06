//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/TreeUtil.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_TREE_UTIL_H
#define LIBBOARDGAME_MCTS_TREE_UTIL_H

#include "ChildIterator.h"
#include "Tree.h"

namespace libboardgame_mcts {
namespace tree_util {

//-----------------------------------------------------------------------------

template<typename M>
const Node<M>* find_child(const Tree<M>& tree, const Node<M>& node, M mv)
{
    for (ChildIterator<M> i(tree, node); i; ++i)
        if (i->get_move() == mv)
            return &(*i);
    return 0;
}

template<typename M, class S>
const Node<M>* find_node(const Tree<M>& tree, const S& sequence)
{
    auto node = &tree.get_root();
    for (M mv : sequence)
        if ((node = find_child(tree, *node, mv)) == 0)
            break;
    return node;
}

//-----------------------------------------------------------------------------

} // namespace tree_util
} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_TREE_UTIL_H
