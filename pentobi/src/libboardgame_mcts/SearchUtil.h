//-----------------------------------------------------------------------------
/** @file SearchUtil.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_SEARCH_UTIL_H
#define LIBBOARDGAME_MCTS_SEARCH_UTIL_H

#include "Search.h"

namespace libboardgame_mcts {
namespace search_util {

//-----------------------------------------------------------------------------

template<class S, class M, class E, unsigned int P>
vector<M> find_principal_variation(const Search<S, M, E, P>& search)
{
    vector<M> result;
    const Node<M>* node = &search.get_tree().get_root();
    while (true)
    {
        node = search.select_child_final(*node);
        if (node == 0)
            break;
        result.push_back(node->get_move());
    }
    return result;
}

//-----------------------------------------------------------------------------

} // namespace search_util
} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_SEARCH_UTIL_H
