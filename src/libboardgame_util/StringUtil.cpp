//-----------------------------------------------------------------------------
/** @file libboardgame_util/StringUtil.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "StringUtil.h"

#include <cctype>
#include <iomanip>
#include <boost/format.hpp>

namespace libboardgame_util {

using boost::format;
using boost::io::group;

//-----------------------------------------------------------------------------

template<>
bool from_string(const string& s, string& t)
{
    t = s;
    return true;
}

string get_letter_coord(unsigned i)
{
    string result;
    while (true)
    {
        result.insert(0, 1, char('a' + i % 26));
        i /= 26;
        if (i == 0)
            break;
        --i;
    }
    return result;
}

vector<string> split(const string& s, char separator)
{
    vector<string> result;
    string current;
    for (char c : s)
    {
        if (c == separator)
        {
            result.push_back(current);
            current.clear();
            continue;
        }
        current.push_back(c);
    }
    if (! current.empty() || ! result.empty())
        result.push_back(current);
    return result;
}

string time_to_string(double seconds, bool with_seconds_as_double)
{
    int int_seconds = int(seconds + 0.5);
    int hours = int_seconds / 3600;
    int_seconds -= hours * 3600;
    int minutes = int_seconds / 60;
    int_seconds -= minutes * 60;
    string result;
    if (hours == 0)
        result = str(format("%1%:%2%")
                     % group(setfill('0'), setw(2), minutes)
                     % group(setfill('0'), setw(2), int_seconds));
    else
        result = str(format("%1%:%2%:%3%")
                     % group(setfill('0'), hours)
                     % group(setfill('0'), setw(2), minutes)
                     % group(setfill('0'), setw(2), int_seconds));
    if (with_seconds_as_double)
        result += str(format(" (%4%)") % seconds);
    return result;
}

string to_lower(const string& s)
{
    string r;
    r.reserve(s.size());
    for (char c : s)
        r.push_back(tolower(c));
    return r;
}

string trim(const string& s)
{
    string::size_type begin = 0;
    auto end = s.size();
    while (begin != end && isspace(s[begin]))
        ++begin;
    while (end > begin && isspace(s[end - 1]))
        --end;
    return s.substr(begin, end - begin);
}

string trim_right(const string& s)
{
    auto end = s.size();
    while (end > 0 && isspace(s[end - 1]))
        --end;
    return s.substr(0, end);
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
