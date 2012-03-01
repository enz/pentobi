//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/Writer.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Writer.h"

#include <cctype>
#include <sstream>

namespace libboardgame_sgf {

using namespace std;

//-----------------------------------------------------------------------------

Writer::Writer(ostream& out, bool one_prop_per_line, unsigned int indent)
    : m_out(out),
      m_one_prop_per_line(one_prop_per_line),
      m_indent(indent),
      m_current_indent(0)
{
}

Writer::~Writer() throw()
{
}

void Writer::begin_node()
{
    m_is_first_prop = true;
    write_indent();
    m_out << ';';
}

void Writer::begin_tree()
{
    write_indent();
    m_out << '(';
    m_current_indent += m_indent;
    if (m_one_prop_per_line)
        m_out << '\n';
}

void Writer::end_node()
{
    if (m_one_prop_per_line && m_is_first_prop)
        m_out << '\n';
}

void Writer::end_tree()
{
    m_current_indent -= m_indent;
    write_indent();
    m_out << ")\n";
}

string Writer::get_escaped(const string& s)
{
    ostringstream buffer;
    BOOST_FOREACH(char c, s)
    {
        if (c == ']' || c == '\\')
            buffer << '\\' << c;
        else if (c != '\n' && isspace(c))
            buffer << ' ';
        else
            buffer << c;
    }
    return buffer.str();
}

void Writer::write_indent()
{
    for (unsigned int i = 0; i < m_current_indent; ++i)
        m_out << ' ';
}

void Writer::write_property(const string& identifier, const format& f)
{
    write_property(identifier, str(f));
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
