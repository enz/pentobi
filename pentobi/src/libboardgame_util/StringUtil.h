//-----------------------------------------------------------------------------
/** @file StringUtil.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_STRING_UTIL_H
#define LIBBOARDGAME_UTIL_STRING_UTIL_H

#include <sstream>
#include <string>

namespace libboardgame_util {
namespace string_util {

using namespace std;

//-----------------------------------------------------------------------------

template<typename T>
bool from_string(const string& s, T& t)
{
    istringstream in(s);
    in >> t;
    return ! in.fail();
}

string time_to_string(double seconds, bool with_seconds_as_double = false);

template<typename T>
string to_string(const T& t)
{
    ostringstream buffer;
    buffer << t;
    return buffer.str();
}

//-----------------------------------------------------------------------------

} // namespace string_util
} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_STRING_UTIL_H
