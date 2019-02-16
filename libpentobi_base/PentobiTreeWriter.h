//-----------------------------------------------------------------------------
/** @file libpentobi_base/PentobiTreeWriter.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_PENTOBI_TREE_WRITER_H
#define LIBPENTOBI_BASE_PENTOBI_TREE_WRITER_H

#include "PentobiTree.h"
#include "libboardgame_base/TreeWriter.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** Blokus-specific tree writer.
    Automatically replaces obsolete move properties as used by early versions
    of Pentobi. */
class PentobiTreeWriter
    : public libboardgame_base::TreeWriter
{
public:
    PentobiTreeWriter(ostream& out, const PentobiTree& tree);

    void write_property(const string& id,
                        const vector<string>& values) override;

private:
    Variant m_variant;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PENTOBI_TREE_WRITER_H
