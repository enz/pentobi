//-----------------------------------------------------------------------------
/** @file TreeUtil.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_TREE_UTIL_H
#define LIBBOARDGAME_MCTS_TREE_UTIL_H

#include "NodeUtil.h"
#include "Tree.h"

namespace libboardgame_mcts {
namespace tree_util {

using libboardgame_mcts::node_util::find_child;

//-----------------------------------------------------------------------------

template<typename M, class S>
const Node<M>* find_node(const Tree<M>& tree, const S& sequence)
{
    const Node<M>* node = &tree.get_root();
    BOOST_FOREACH(M mv, sequence)
        if ((node = find_child(*node, mv)) == 0)
            break;
    return node;
}

//-----------------------------------------------------------------------------

} // namespace tree_util
} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_TREE_UTIL_H
