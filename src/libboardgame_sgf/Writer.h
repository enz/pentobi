//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/Writer.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_WRITER_H
#define LIBBOARDGAME_SGF_WRITER_H

#include <iosfwd>
#include <string>
#include <vector>
#include "libboardgame_util/StringUtil.h"

namespace libboardgame_sgf {

using namespace std;
using libboardgame_util::to_string;

//-----------------------------------------------------------------------------

class Writer
{
public:
    explicit Writer(ostream& out);

    /** @name Formatting options.
        Should be set before starting to write. */
    /** @{ */

    void set_one_prop_per_line(bool enable);

    void set_one_prop_value_per_line(bool enable);

    /** @param indent The number of spaces to indent subtrees, -1 means
        to not even use newlines. */
    void set_indent(int indent);

    /** @} */ // @name


    void begin_tree();

    void end_tree();

    void begin_node();

    void end_node();

    void write_property(const string& id, const char* value);

    template<typename T>
    void write_property(const string& id, const T& value);

    template<typename T>
    void write_property(const string& id, const vector<T>& values);

private:
    ostream& m_out;

    bool m_one_prop_per_line = false;

    bool m_one_prop_value_per_line = false;

    bool m_is_first_prop;

    int m_indent = 0;

    unsigned m_current_indent = 0;

    unsigned m_level = 0;


    static string get_escaped(const string& s);

    void write_indent();
};

inline void Writer::set_one_prop_per_line(bool enable)
{
    m_one_prop_per_line = enable;
}

inline void Writer::set_one_prop_value_per_line(bool enable)
{
    m_one_prop_value_per_line = enable;
}

inline void Writer::set_indent(int indent)
{
    m_indent = indent;
}

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
    if (m_one_prop_per_line && ! m_is_first_prop)
    {
        write_indent();
        m_out << ' ';
    }
    m_out << id;
    bool is_first_value = true;
    for (auto& i : values)
    {
        if (m_one_prop_per_line && m_one_prop_value_per_line
                && ! is_first_value && m_indent >= 0)
        {
            m_out << '\n';
            auto indent = m_current_indent + 1 + id.size();
            for (unsigned i = 0; i < indent; ++i)
                m_out << ' ';
        }
        m_out << '[' << get_escaped(to_string(i)) << ']';
        is_first_value = false;
    }
    if (m_one_prop_per_line && m_indent >= 0)
        m_out << '\n';
    m_is_first_prop = false;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_WRITER_H
