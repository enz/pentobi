//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/TreeUtil.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_TREE_UTIL_H
#define LIBBOARDGAME_MCTS_TREE_UTIL_H

#include "Tree.h"

namespace libboardgame_mcts {

//-----------------------------------------------------------------------------

template<typename N>
const N* find_child(const Tree<N>& tree, const N& node, typename N::Move mv)
{
    for (auto& i : tree.get_children(node))
        if (i.get_move() == mv)
            return &i;
    return nullptr;
}

template<typename N, class S>
const N* find_node(const Tree<N>& tree, const S& sequence)
{
    auto node = &tree.get_root();
    for (auto mv : sequence)
        if (! ((node = find_child(tree, *node, mv))))
            break;
    return node;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_TREE_UTIL_H
