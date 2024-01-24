//-----------------------------------------------------------------------------
/** @file libboardgame_base/TreeReader.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "TreeReader.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

unique_ptr<SgfNode> TreeReader::move_tree()
{
    return std::move(m_root);
}

void TreeReader::on_begin_tree(bool is_root)
{
    if (! is_root)
        m_stack.push(m_current);
}

void TreeReader::on_end_tree(bool is_root)
{
    if (! is_root)
    {
        LIBBOARDGAME_ASSERT(! m_stack.empty());
        m_current = m_stack.top();
        m_stack.pop();
    }
}

void TreeReader::on_begin_node(bool is_root)
{
    if (is_root)
    {
        m_root = make_unique<SgfNode>();
        m_current = m_root.get();
    }
    else
        m_current = &m_current->create_new_child();
}

void TreeReader::on_end_node()
{
}

void TreeReader::on_property(const string& identifier,
                             const vector<string>& values)
{
    m_current->set_property(identifier, values);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
