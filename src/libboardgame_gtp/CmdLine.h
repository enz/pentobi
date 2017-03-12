//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/CmdLine.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_GTP_CMDLINE_H
#define LIBBOARDGAME_GTP_CMDLINE_H

#include <algorithm>
#include <cassert>
#include <string>
#include <iterator>
#include <vector>
#include "CmdLineRange.h"

namespace libboardgame_gtp {

using namespace std;

//-----------------------------------------------------------------------------

/** Parsed GTP command line.
    Only used internally by libboardgame_gtp::Engine. GTP command handlers
    query arguments of the command line through the instance of class Arguments
    given as a function argument by class Engine to the command handler. */
class CmdLine
{
public:
    /** Construct empty command.
        @warning An empty command cannot be used, before init() was called.
        This constructor exists only to reuse instances. */
    CmdLine() = default;

    /** Construct with a command line.
        @see init() */
    explicit CmdLine(const string& line);

    ~CmdLine();

    void init(const string& line);

    void init(const CmdLine& c);

    const string& get_line() const;

    /** Get command name. */
    CmdLineRange get_name() const;

    void write_id(ostream& out) const;

    CmdLineRange get_trimmed_line_after_elem(unsigned i) const;

    const vector<CmdLineRange>& get_elements() const;

    const CmdLineRange& get_element(unsigned i) const;

    int get_idx_name() const;

private:
    int m_idx_name;

    /** Full command line. */
    string m_line;

    vector<CmdLineRange> m_elem;

    void add_elem(string::const_iterator begin, string::const_iterator end);

    void find_elem();

    void parse_id();
};

inline const vector<CmdLineRange>& CmdLine::get_elements() const
{
    return m_elem;
}

inline const CmdLineRange& CmdLine::get_element(unsigned i) const
{
    assert(i < m_elem.size());
    return m_elem[i];
}

inline int CmdLine::get_idx_name() const
{
    return m_idx_name;
}

inline const string& CmdLine::get_line() const
{
    return m_line;
}

inline CmdLineRange CmdLine::get_name() const
{
    return m_elem[m_idx_name];
}

inline void CmdLine::write_id(ostream& out) const
{
    if (m_idx_name == 0)
        return;
    auto& e = m_elem[0];
    copy(e.begin(), e.end(), ostream_iterator<char>(out));
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp

#endif // LIBBOARDGAME_GTP_CMDLINE_H
