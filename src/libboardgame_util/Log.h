//-----------------------------------------------------------------------------
/** @file libboardgame_util/Log.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_LOG_H
#define LIBBOARDGAME_UTIL_LOG_H

#include <sstream>
#include <string>

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

ostream& get_log();

void log(const string& s);

template<typename T>
void log_buffered(ostream& buffer, const T& t)
{
    buffer << t;
}

template<typename T, typename... Ts>
void log_buffered(ostream& buffer, const T& first, const Ts&... rest)
{
    buffer << first;
    log_buffered(buffer, rest...);
}

/** Write a number of arguments to the log stream.
    Writes to a buffer first so there is only a single write to the log
    stream. Appends a newline. */
template<typename... Ts>
void log(const Ts&... args)
{
    ostringstream buffer;
    log_buffered(buffer, args...);
    log(buffer.str());
}

void set_log(ostream& out);

void set_log_null();

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_LOG_H
