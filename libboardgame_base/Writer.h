//-----------------------------------------------------------------------------
/** @file libboardgame_base/Writer.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_WRITER_H
#define LIBBOARDGAME_BASE_WRITER_H

#include <iosfwd>
#include <string>
#include <vector>
#include "StringUtil.h"

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

class Writer
{
public:
    explicit Writer(ostream& out);

    /** @name Formatting options.
        Should be set before starting to write. */
    /** @{ */

    /** @param indent The number of spaces to indent subtrees, -1 means
        to not even use newlines. */
    void set_indent(int indent) { m_indent = indent; }

    /** @} */ // @name


    void begin_tree();

    void end_tree();

    void begin_node();

    void end_node() { }

    void write_property(const string& id, const char* value);

    template<typename T>
    void write_property(const string& id, const T& value);

    template<typename T>
    void write_property(const string& id, const vector<T>& values);

private:
    ostream& m_out;

    bool m_is_first_prop;

    int m_indent = 0;

    unsigned m_current_indent = 0;

    unsigned m_level = 0;


    static string get_escaped(const string& s);

    void write_indent();
};

inline void Writer::write_property(const string& id, const char* value)
{
    vector<const char*> values(1, value);
    write_property(id, values);
}

template<typename T>
void Writer::write_property(const string& id, const T& value)
{
    vector<T> values(1, value);
    write_property(id, values);
}

template<typename T>
void Writer::write_property(const string& id, const vector<T>& values)
{
    m_out << id;
    bool is_first_value = true;
    for (auto& i : values)
    {
        m_out << '[' << get_escaped(to_string(i)) << ']';
        is_first_value = false;
    }
    m_is_first_prop = false;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_WRITER_H
