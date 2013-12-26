//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/CmdLineRange.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_GTP_CMDLINERANGE_H
#define LIBBOARDGAME_GTP_CMDLINERANGE_H

#include <iosfwd>
#include <algorithm>
#include <string>

namespace libboardgame_gtp {

using namespace std;

//-----------------------------------------------------------------------------

/** Subrange of the GTP command line.
    Avoids allocation of strings on the heap for each parsed command line.
    Instances of this class are valid only during the lifetime of the command
    line object. Command handlers, which access the command line through the
    instance of Arguments given as a function argument, should not store
    references to CmdLineRange objects. */
class CmdLineRange
{
public:
    CmdLineRange(string::const_iterator begin, string::const_iterator end);

    bool operator==(const string& s) const;

    bool operator!=(const string& s) const;

    operator string() const;

    string::const_iterator begin() const;

    string::const_iterator end() const;

    string::size_type size() const;

    void write(ostream& o) const;

private:
    string::const_iterator m_begin;

    string::const_iterator m_end;
};

inline CmdLineRange::CmdLineRange(string::const_iterator begin,
                                  string::const_iterator end)
    : m_begin(begin),
      m_end(end)
{
}

inline bool CmdLineRange::operator==(const string& s) const
{
    return size() == s.size() && equal(m_begin, m_end, s.begin());
}

inline bool CmdLineRange::operator!=(const string& s) const
{
    return ! operator==(s);
}

inline string::const_iterator CmdLineRange::begin() const
{
    return m_begin;
}

inline string::const_iterator CmdLineRange::end() const
{
    return m_end;
}

inline string::size_type CmdLineRange::size() const
{
    return m_end - m_begin;
}

//-----------------------------------------------------------------------------

inline ostream& operator<<(ostream& out, const CmdLineRange& r)
{
    r.write(out);
    return out;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp

#endif // LIBBOARDGAME_GTP_CMDLINERANGE_H
