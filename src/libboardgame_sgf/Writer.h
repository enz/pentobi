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
           unsigned int indent = 0);

    ~Writer() throw();

    void begin_tree();

    void end_tree();

    void begin_node();

    void end_node();

    template<typename T>
    void write_property(const string& identifier, const T& value);

    void write_property(const string& identifier, const format& f);

    template<typename T>
    void write_property(const string& identifier, const vector<T>& values);

private:
    ostream& m_out;

    bool m_one_prop_per_line;

    bool m_is_first_prop;

    unsigned int m_indent;

    unsigned int m_current_indent;

    static string get_escaped(const string& s);

    void write_indent();
};

template<typename T>
void Writer::write_property(const string& identifier, const T& value)
{
    if (m_one_prop_per_line && ! m_is_first_prop)
    {
        write_indent();
        m_out << ' ';
    }
    m_out << identifier << '[' << get_escaped(to_string(value)) << ']';
    if (m_one_prop_per_line)
        m_out << '\n';
    m_is_first_prop = false;
}

template<typename T>
void Writer::write_property(const string& identifier, const vector<T>& values)
{
    if (m_one_prop_per_line && ! m_is_first_prop)
    {
        write_indent();
        m_out << ' ';
    }
    m_out << identifier;
    BOOST_FOREACH(const T& i, values)
        m_out << '[' << get_escaped(to_string(i)) << ']';
    if (m_one_prop_per_line)
        m_out << '\n';
    m_is_first_prop = false;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_WRITER_H
