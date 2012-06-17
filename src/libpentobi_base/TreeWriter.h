//-----------------------------------------------------------------------------
/** @file libpentobi_base/TreeWriter.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_TREE_WRITER_H
#define LIBPENTOBI_BASE_TREE_WRITER_H

#include "Tree.h"
#include "libboardgame_sgf/TreeWriter.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** Blokus-specific tree writer.
    Automatically replaces obsolete move properties as used by early versions
    of Pentobi. */
class TreeWriter
    : public libboardgame_sgf::TreeWriter
{
public:
    TreeWriter(ostream& out, const Tree& tree);

    virtual ~TreeWriter();

    void write_property(const string& id, const vector<string>& values);

private:
    Variant m_variant;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_TREE_WRITER_H
