//-----------------------------------------------------------------------------
/** @file libpentobi_base/TreeWriter.cpp
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TreeWriter.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

TreeWriter::TreeWriter(ostream& out, const Tree& tree)
    : libboardgame_sgf::TreeWriter(out, tree.get_root()),
      m_variant(tree.get_variant())
{
}

TreeWriter::~TreeWriter()
{
}

void TreeWriter::write_property(const string& id, const vector<string>& values)
{
    // Replace obsolete move property IDs or multi-valued move properties
    // as used by early versions of Pentobi
    if (id == "BLUE" || id == "YELLOW" || id == "GREEN" || id == "RED"
        || ((id == "1" || id == "2" || id == "3" || id == "4" || id == "B"
             || id == "W")
            && values.size() > 1))
    {
        string new_id;
        if (id == "BLUE")
            new_id = "1";
        else if (id == "YELLOW")
            new_id = "2";
        else if (id == "GREEN")
        {
            if (m_variant == Variant::duo || m_variant == Variant::junior)
                new_id = "2";
            else
                new_id = "4";
        }
        else if (id == "RED")
            new_id = "3";
        else
            new_id = id;
        if (values.size() < 2)
            libboardgame_sgf::TreeWriter::write_property(new_id, values);
        else
        {
            string val = values[0];
            for (size_t i = 1; i < values.size(); ++i)
                val += "," + values[i];
            vector<string> new_values;
            new_values.push_back(val);
            libboardgame_sgf::TreeWriter::write_property(new_id, new_values);
        }
    }
    else
        libboardgame_sgf::TreeWriter::write_property(id, values);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
