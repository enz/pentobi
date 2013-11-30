//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/TreeReader.cpp
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TreeReader.h"

namespace libboardgame_sgf {

using namespace std;

//-----------------------------------------------------------------------------

TreeReader::TreeReader()
    : m_current(nullptr)
{
}

TreeReader::~TreeReader() throw()
{
}

unique_ptr<Node> TreeReader::get_tree_transfer_ownership()
{
    return move(m_root);
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
        m_root.reset(new Node());
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

} // namespace libboardgame_sgf
