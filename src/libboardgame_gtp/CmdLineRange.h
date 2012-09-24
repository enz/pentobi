//-----------------------------------------------------------------------------
/** @file CmdLineRange.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_GTP_CMDLINERANGE_H
#define LIBBOARDGAME_GTP_CMDLINERANGE_H

#include <iosfwd>
#include <string>
#include <boost/range.hpp>

namespace libboardgame_gtp {

using namespace std;
using boost::iterator_range;
using boost::make_iterator_range;
using boost::sub_range;

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

    bool operator<(const CmdLineRange& r) const;

    operator string() const;

    string::const_iterator begin() const;

    string::const_iterator end() const;

    void write(ostream& o) const;

private:
    sub_range<const string> m_range;
};

inline CmdLineRange::CmdLineRange(string::const_iterator begin,
                                  string::const_iterator end)
    : m_range(begin, end)
{
}

inline bool CmdLineRange::operator==(const string& s) const
{
    return m_range == sub_range<const string>(s.begin(), s.end());
}

inline bool CmdLineRange::operator!=(const string& s) const
{
    return ! operator==(s);
}

inline bool CmdLineRange::operator<(const CmdLineRange& r) const
{
    return m_range < r.m_range;
}

inline string::const_iterator CmdLineRange::begin() const
{
    return m_range.begin();
}

inline string::const_iterator CmdLineRange::end() const
{
    return m_range.end();
}

inline void CmdLineRange::write(ostream& o) const
{
    o << m_range;
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
