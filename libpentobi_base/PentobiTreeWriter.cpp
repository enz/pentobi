//-----------------------------------------------------------------------------
/** @file libpentobi_base/PentobiTreeWriter.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "PentobiTreeWriter.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

PentobiTreeWriter::PentobiTreeWriter(ostream& out, const PentobiTree& tree)
    : libboardgame_base::TreeWriter(out, tree.get_root()),
      m_variant(tree.get_variant())
{
}

void PentobiTreeWriter::write_property(const string& id,
                                       const vector<string>& values)
{
    auto nu_colors = get_nu_colors(m_variant);
    // Replace obsolete move property IDs or multi-valued move properties
    // as used by early versions of Pentobi
    if (id == "BLUE" || id == "YELLOW" || id == "GREEN" || id == "RED"
        || ((id == "1" || id == "2" || id == "3" || id == "4" || id == "B"
             || id == "W")
            && values.size() > 1))
    {
        string new_id;
        if (id == "BLUE")
            new_id = (nu_colors == 2 ? "B" : "1");
        else if (id == "YELLOW")
            new_id = "2";
        else if (id == "GREEN")
            new_id = (nu_colors == 2 ? "W" : "4");
        else if (id == "RED")
            new_id = "3";
        else
            new_id = id;
        if (values.size() < 2)
            libboardgame_base::TreeWriter::write_property(new_id, values);
        else
        {
            string val = values[0];
            for (size_t i = 1; i < values.size(); ++i)
                val += "," + values[i];
            vector<string> new_values;
            new_values.push_back(val);
            libboardgame_base::TreeWriter::write_property(new_id, new_values);
        }
        return;
    }
    // Pentobi 12.0 versions erroneously used multi-player properties for
    // two-player Callisto.
    if (nu_colors == 2)
    {
        if (id == "1")
        {
            libboardgame_base::TreeWriter::write_property("B", values);
            return;
        }
        if (id == "2")
        {
            libboardgame_base::TreeWriter::write_property("W", values);
            return;
        }
    }
    libboardgame_base::TreeWriter::write_property(id, values);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
