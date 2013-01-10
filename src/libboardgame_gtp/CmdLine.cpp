//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/CmdLine.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "CmdLine.h"

#include <cassert>
#include <boost/algorithm/string/trim.hpp>

namespace libboardgame_gtp {

using namespace std;

//-----------------------------------------------------------------------------

CmdLine::~CmdLine() throw()
{
}

/** Find elements (ID, command name, arguments).
    Arguments are words separated by whitespaces.
    Arguments with whitespaces can be quoted with quotation marks ('"').
    Characters can be escaped with a backslash ('\'). */
void CmdLine::find_elem()
{
    m_elem.clear();
    bool escape = false;
    bool is_in_string = false;
    string::const_iterator begin = m_line.begin();
    string::const_iterator i;
    for (i = begin; i < m_line.end(); ++i)
    {
        char c = *i;
        if (c == '"' && ! escape)
        {
            if (is_in_string)
                m_elem.push_back(CmdLineRange(begin, i));
            begin = i + 1;
            is_in_string = ! is_in_string;
        }
        else if (isspace(static_cast<unsigned char>(c)) && ! is_in_string)
        {
            if (i > begin)
                m_elem.push_back(CmdLineRange(begin, i));
            begin = i + 1;
        }
        escape = (c == '\\' && ! escape);
    }
    if (i > begin)
        m_elem.push_back(CmdLineRange(begin, m_line.end()));
}

CmdLineRange CmdLine::get_trimmed_line_after_elem(size_t i) const
{
    assert(i < m_elem.size());
    const CmdLineRange& e = m_elem[i];
    auto begin = e.end();
    if (begin < m_line.end() && *begin == '"')
        ++begin;
    while (begin < m_line.end() && isspace(static_cast<unsigned char>(*begin)))
        ++begin;
    auto end = m_line.end();
    while (end > begin && isspace(static_cast<unsigned char>(*(end - 1))))
        --end;
    return CmdLineRange(begin, end);
}

void CmdLine::init(const string& line)
{
    m_line = line;
    find_elem();
    assert(! m_elem.empty());
    parse_id();
    assert(! m_elem.empty());
}

void CmdLine::init(const CmdLine& c)
{
    m_idx_name = c.m_idx_name;
    m_line = c.m_line;
    m_elem.clear();
    for (auto i = c.m_elem.begin(); i != c.m_elem.end(); ++i)
    {
        auto begin = m_line.begin() + (i->begin() - c.m_line.begin());
        auto end = m_line.begin() + (i->end() - c.m_line.begin());
        m_elem.push_back(CmdLineRange(begin, end));
    }
}

void CmdLine::parse_id()
{
    m_idx_name = 0;
    if (m_elem.size() < 2)
        return;
    istringstream in(m_elem[0]);
    int id;
    in >> id;
    if (in)
        m_idx_name = 1;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp
