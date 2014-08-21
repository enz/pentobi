//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/ChildIterator.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_CHILD_ITERATOR_H
#define LIBBOARDGAME_MCTS_CHILD_ITERATOR_H

#include "Tree.h"

namespace libboardgame_mcts {

using namespace std;

//-----------------------------------------------------------------------------

/** Iterate over all children of a node in the MCTS tree.
    @tparam M The type of the move stored in the nodes. */
template<typename N>
class ChildIterator
{
public:
    typedef N Node;

    typedef libboardgame_mcts::Tree<Node> Tree;

    ChildIterator(const Tree& tree, const Node& node);

    operator bool() const;

    ChildIterator& operator++();

    const Node& operator*();

    const Node* operator->();

private:
    const Node* m_current;

    const Node* m_end;
};

template<typename N>
ChildIterator<N>::ChildIterator(const Tree& tree, const Node& node)
{
    auto nu_children = node.get_nu_children();
    m_current =
        (nu_children != 0 ? &tree.get_node(node.get_first_child()) : nullptr);
    m_end = m_current + nu_children;
}

template<typename N>
inline ChildIterator<N>::operator bool() const
{
    return m_current != m_end;
}

template<typename N>
inline ChildIterator<N>& ChildIterator<N>::operator++()
{
    LIBBOARDGAME_ASSERT(*this);
    ++m_current;
    return *this;
}

template<typename N>
inline auto ChildIterator<N>::operator*() -> const Node&
{
    LIBBOARDGAME_ASSERT(*this);
    return *m_current;
}

template<typename N>
inline auto ChildIterator<N>::operator->() -> const Node*
{
    LIBBOARDGAME_ASSERT(*this);
    return m_current;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_CHILD_ITERATOR_H
