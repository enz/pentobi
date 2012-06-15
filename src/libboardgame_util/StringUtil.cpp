//-----------------------------------------------------------------------------
/** @file libboardgame_util/StringUtil.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "StringUtil.h"

#include <iomanip>
#include <boost/format.hpp>

namespace libboardgame_util {
namespace string_util {

using boost::format;
using boost::io::group;

//-----------------------------------------------------------------------------

template<>
bool from_string(const string& s, string& t)
{
    t = s;
    return true;
}

string get_letter_coord(unsigned int i)
{
    string result;
    ++i;
    unsigned int n = 1;
    while (n * 26 < i)
        n *= 26;
    while (true)
    {
        unsigned int d = i / n;
        result += char('a' + d - 1);
        if (n == 1)
            break;
        i -= n * d;
        n /= 26;
    }
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

//----------------------------------------------------------------------------

} // namespace string_util
} // namespace libboardgame_util
