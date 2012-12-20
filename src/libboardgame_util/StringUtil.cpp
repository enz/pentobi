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
