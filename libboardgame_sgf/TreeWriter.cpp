//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/TreeWriter.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "TreeWriter.h"

namespace libboardgame_sgf {

//-----------------------------------------------------------------------------

TreeWriter::TreeWriter(ostream& out, const SgfNode& root)
    : m_root(root),
      m_writer(out)
{
}

void TreeWriter::write()
{
    m_writer.begin_tree();
    write_node(m_root);
    m_writer.end_tree();
}

void TreeWriter::write_node(const SgfNode& node)
{
    m_writer.begin_node();
    for (auto& i : node.get_properties())
        write_property(i.id, i.values);
    m_writer.end_node();
    if (! node.has_children())
        return;
    if (node.has_single_child())
        write_node(node.get_child());
    else
        for (auto& i : node.get_children())
        {
            m_writer.begin_tree();
            write_node(i);
            m_writer.end_tree();
        }
}

void TreeWriter::write_property(const string& id, const vector<string>& values)
{
    m_writer.write_property(id, values);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
