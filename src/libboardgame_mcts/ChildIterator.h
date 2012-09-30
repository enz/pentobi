//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/ChildIterator.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_CHILD_ITERATOR_H
#define LIBBOARDGAME_MCTS_CHILD_ITERATOR_H

#include "Node.h"

namespace libboardgame_mcts {

using namespace std;

//-----------------------------------------------------------------------------

/** Iterate over all children of a node in the MCTS tree.
    @tparam M The type of the move stored in the nodes. */
template<typename M>
class ChildIterator
{
public:
    ChildIterator(const Node<M>& node);

    operator bool() const;

    void operator++();

    const Node<M>& operator*();

    const Node<M>* operator->();

private:
    const Node<M>* m_current;

    const Node<M>* m_end;
};

template<typename M>
ChildIterator<M>::ChildIterator(const Node<M>& node)
{
    m_current = node.get_first_child();
    if (m_current != 0)
        m_end = m_current + node.get_nu_children();
    else
        m_end = 0;
}

template<typename M>
inline ChildIterator<M>::operator bool() const
{
    return m_current != m_end;
}

template<typename M>
inline void ChildIterator<M>::operator++()
{
    LIBBOARDGAME_ASSERT(operator bool());
    ++m_current;
}

template<typename M>
inline const Node<M>& ChildIterator<M>::operator*()
{
    LIBBOARDGAME_ASSERT(operator bool());
    return *m_current;
}

template<typename M>
inline const Node<M>* ChildIterator<M>::operator->()
{
    LIBBOARDGAME_ASSERT(operator bool());
    return m_current;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_CHILD_ITERATOR_H
