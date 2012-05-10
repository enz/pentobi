//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/Writer.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_WRITER_H
#define LIBBOARDGAME_SGF_WRITER_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include "libboardgame_util/StringUtil.h"

namespace libboardgame_sgf {

using namespace std;
using boost::format;
using libboardgame_util::string_util::to_string;

//-----------------------------------------------------------------------------

class Writer
{
public:
    Writer(ostream& out, bool one_prop_per_line = false,
           bool one_prop_value_per_line = false,
           unsigned int indent = 0);

    ~Writer() throw();

    void begin_tree();

    void end_tree();

    void begin_node();

    void end_node();

    void write_property(const string& id, const char* value);

    template<typename T>
    void write_property(const string& id, const T& value);

    void write_property(const string& id, const format& f);

    template<typename T>
    void write_property(const string& id, const vector<T>& values);

private:
    ostream& m_out;

    bool m_one_prop_per_line;

    bool m_one_prop_value_per_line;

    bool m_is_first_prop;

    unsigned int m_indent;

    unsigned int m_current_indent;

    unsigned int m_level;

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
    if (m_one_prop_per_line && ! m_is_first_prop)
    {
        write_indent();
        m_out << ' ';
    }
    m_out << id;
    bool is_first_value = true;
    BOOST_FOREACH(const T& i, values)
    {
        if (m_one_prop_per_line && m_one_prop_value_per_line
            && ! is_first_value)
        {
            m_out << '\n';
            unsigned int indent =
                static_cast<unsigned int>(m_current_indent + 1 + id.size());
            for (unsigned int i = 0; i < indent; ++i)
                m_out << ' ';
        }
        m_out << '[' << get_escaped(to_string(i)) << ']';
        is_first_value = false;
    }
    if (m_one_prop_per_line)
        m_out << '\n';
    m_is_first_prop = false;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_WRITER_H
