//-----------------------------------------------------------------------------
/** @file libboardgame_base/StringUtil.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "StringUtil.h"

#include <cctype>
#include <cmath>
#include <iomanip>

namespace libboardgame_base {

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
    auto int_seconds = static_cast<int>(round(seconds));
    int hours = int_seconds / 3600;
    int_seconds -= hours * 3600;
    int minutes = int_seconds / 60;
    int_seconds -= minutes * 60;
    ostringstream s;
    s << setfill('0');
    if (hours > 0)
        s << hours << ':';
    s << setw(2) << minutes << ':' << setw(2) << int_seconds;
    if (with_seconds_as_double)
        s << " (" << seconds << ')';
    return s.str();
}

string to_lower(string s)
{
    for (auto& c : s)
        c = static_cast<char>(tolower(c));
    return s;
}

string trim(const string& s)
{
    string::size_type begin = 0;
    auto end = s.size();
    while (begin != end && isspace(s[begin]) != 0)
        ++begin;
    while (end > begin && isspace(s[end - 1]) != 0)
        --end;
    return s.substr(begin, end - begin);
}

//----------------------------------------------------------------------------

} // namespace libboardgame_base
