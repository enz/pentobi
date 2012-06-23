//-----------------------------------------------------------------------------
/** @file NullExplorationTerm.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_NULL_EXPLORATION_TERM_H
#define LIBBOARDGAME_MCTS_NULL_EXPLORATION_TERM_H

#include "Node.h"

namespace libboardgame_mcts {

//-----------------------------------------------------------------------------

template<class M>
class NullExplorationTerm
{
public:
    typedef M Move;

    typedef libboardgame_mcts::Node<M> Node;

    void begin_select_child(const Node& parent);

    Float get(const Node& child);
};

template<class M>
inline void NullExplorationTerm<M>::begin_select_child(const Node& parent)
{
    LIBBOARDGAME_UNUSED(parent);
}

template<class M>
inline Float NullExplorationTerm<M>::get(const Node& child)
{
    LIBBOARDGAME_UNUSED(child);
    return 0;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_NULL_EXPLORATION_TERM_H
