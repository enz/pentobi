//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/TreeIterator.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TreeIterator.h"

namespace libboardgame_sgf {

//-----------------------------------------------------------------------------

void TreeIterator::operator++()
{
    LIBBOARDGAME_ASSERT(operator bool());
    const Node* first_child = m_current->get_first_child_or_null();
    if (first_child != 0)
    {
        m_current = first_child;
        return;
    }
    if (m_current == &m_root)
    {
        m_current = 0;
        return;
    }
    while (m_current->get_sibling() == 0)
    {
        m_current = &m_current->get_parent();
        if (m_current == &m_root)
        {
            m_current = 0;
            return;
        }
    }
    m_current = m_current->get_sibling();
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
