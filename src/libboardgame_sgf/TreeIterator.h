//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/TreeIterator.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_TREE_ITERATOR_H
#define LIBBOARDGAME_SGF_TREE_ITERATOR_H

#include "Node.h"

namespace libboardgame_sgf {

//-----------------------------------------------------------------------------

class TreeIterator
{
public:
    TreeIterator(const Node& root);

    operator bool() const;

    const Node& operator*() const;

    void operator++();

private:
    const Node& m_root;

    const Node* m_current;
};

inline TreeIterator::TreeIterator(const Node& root)
  : m_root(root),
    m_current(&root)
{
}

inline TreeIterator::operator bool() const
{
    return m_current != nullptr;
}

inline const Node& TreeIterator::operator*() const
{
    LIBBOARDGAME_ASSERT(operator bool());
    return *m_current;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_TREE_ITERATOR_H
