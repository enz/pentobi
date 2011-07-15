//-----------------------------------------------------------------------------
/** @file ChildIterator.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_CHILD_ITERATOR_H
#define LIBBOARDGAME_MCTS_CHILD_ITERATOR_H

#include "Node.h"

namespace libboardgame_mcts {

using namespace std;

//-----------------------------------------------------------------------------

template<typename MOVE>
class ChildIterator
{
public:
    ChildIterator(const Node<MOVE>& node);

    operator bool() const;

    void operator++();

    const Node<MOVE>& operator*();

    const Node<MOVE>* operator->();

private:
    const Node<MOVE>* m_current;

    const Node<MOVE>* m_end;
};

template<typename MOVE>
ChildIterator<MOVE>::ChildIterator(const Node<MOVE>& node)
{
    m_current = node.get_first_child();
    if (m_current != 0)
        m_end = m_current + node.get_nu_children();
    else
        m_end = 0;
}

template<typename MOVE>
inline ChildIterator<MOVE>::operator bool() const
{
    return m_current != m_end;
}

template<typename MOVE>
inline void ChildIterator<MOVE>::operator++()
{
    LIBBOARDGAME_ASSERT(operator bool());
    ++m_current;
}

template<typename MOVE>
inline const Node<MOVE>& ChildIterator<MOVE>::operator*()
{
    LIBBOARDGAME_ASSERT(operator bool());
    return *m_current;
}

template<typename MOVE>
inline const Node<MOVE>* ChildIterator<MOVE>::operator->()
{
    LIBBOARDGAME_ASSERT(operator bool());
    return m_current;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_CHILD_ITERATOR_H
