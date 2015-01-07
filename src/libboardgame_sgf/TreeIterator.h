//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/TreeIterator.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_TREE_ITERATOR_H
#define LIBBOARDGAME_SGF_TREE_ITERATOR_H

#include "SgfNode.h"

namespace libboardgame_sgf {

//-----------------------------------------------------------------------------

class TreeIterator
{
public:
    TreeIterator(const SgfNode& root);

    operator bool() const;

    const SgfNode& operator*() const;

    void operator++();

private:
    const SgfNode& m_root;

    const SgfNode* m_current;
};

inline TreeIterator::TreeIterator(const SgfNode& root)
  : m_root(root),
    m_current(&root)
{
}

inline TreeIterator::operator bool() const
{
    return m_current;
}

inline const SgfNode& TreeIterator::operator*() const
{
    LIBBOARDGAME_ASSERT(*this);
    return *m_current;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_TREE_ITERATOR_H
