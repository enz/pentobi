//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/CmdLine.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "CmdLine.h"

#include <limits>
#include <sstream>

namespace libboardgame_gtp {

//-----------------------------------------------------------------------------

CmdLine::CmdLine(const string& line)
{
    init(line);
}

CmdLine::~CmdLine() = default; // Non-inline to avoid GCC -Winline warning

void CmdLine::add_elem(string::const_iterator begin,
                       string::const_iterator end)
{
    // Ignore command line elements greater UINT_MAX because we use unsigned
    // for element indices.
    if (m_elem.size() < numeric_limits<unsigned>::max())
        m_elem.emplace_back(&*begin, end - begin);
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
                add_elem(begin, i);
            begin = i + 1;
            is_in_string = ! is_in_string;
        }
        else if (isspace(static_cast<unsigned char>(c)) != 0 && ! is_in_string)
        {
            if (i > begin)
                m_elem.emplace_back(&*begin, i - begin);
            begin = i + 1;
        }
        escape = (c == '\\' && ! escape);
    }
    if (i > begin)
        m_elem.emplace_back(&*begin, m_line.end() - begin);
}

string_view CmdLine::get_trimmed_line_after_elem(unsigned i) const
{
    assert(i < m_elem.size());
    auto& e = m_elem[i];
    auto begin = e.end();
    auto end = &*m_line.end();
    if (begin < end && *begin == '"')
        ++begin;
    while (begin < end && isspace(static_cast<unsigned char>(*begin)) != 0)
        ++begin;
    while (end > begin && isspace(static_cast<unsigned char>(*(end - 1))) != 0)
        --end;
    return {begin, static_cast<string_view::size_type>(end - begin)};
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
    for (auto& i : c.m_elem)
    {
        auto begin = m_line.begin() + (&*i.begin() - &*c.m_line.begin());
        auto end = m_line.begin() + (&*i.end() - &*c.m_line.begin());
        m_elem.emplace_back(&*begin, end - begin);
    }
}

void CmdLine::parse_id()
{
    m_idx_name = 0;
    if (m_elem.size() < 2)
        return;
    istringstream in(string(&*m_elem[0].begin(), m_elem[0].size()));
    int id;
    in >> id;
    if (in)
        m_idx_name = 1;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp
