//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/TreeWriter.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_TREE_WRITER_H
#define LIBBOARDGAME_SGF_TREE_WRITER_H

#include "Node.h"
#include "Writer.h"

namespace libboardgame_sgf {

//-----------------------------------------------------------------------------

class TreeWriter
{
public:
    TreeWriter(ostream& out, const Node& root);

    virtual ~TreeWriter();

    /** Overridable function to write a property.
        Can be used in subclasses, for example, to replace or remove obsolete
        properties or do other sanitizing. */
    virtual void write_property(const string& id,
                                const vector<string>& values);


    /** @name Formatting options.
        Should be set before starting to write. */
    // @{

    void set_one_prop_per_line(bool enable);

    void set_one_prop_value_per_line(bool enable);

    void set_indent(unsigned int indent);

    // @} // @name


    void write();

private:
    const Node& m_root;

    Writer m_writer;

    void write_node(const Node& node);
};

inline void TreeWriter::set_one_prop_per_line(bool enable)
{
    m_writer.set_one_prop_per_line(enable);
}

inline void TreeWriter::set_one_prop_value_per_line(bool enable)
{
    m_writer.set_one_prop_value_per_line(enable);
}

inline void TreeWriter::set_indent(unsigned int indent)
{
    m_writer.set_indent(indent);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_TREE_WRITER_H
