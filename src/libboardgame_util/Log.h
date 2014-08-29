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

/** Write a string to the log stream.
    Appends a newline if the output has no newline at the end. */
void log(const string& s);

/** Helper function needed for log(const Ts&...) */
template<typename T>
void log_buffered(ostream& buffer, const T& t)
{
    buffer << t;
}

/** Helper function needed for log(const Ts&...) */
template<typename T, typename... Ts>
void log_buffered(ostream& buffer, const T& first, const Ts&... rest)
{
    buffer << first;
    log_buffered(buffer, rest...);
}

/** Write a number of arguments to the log stream.
    Writes to a buffer first so there is only a single write to the log
    stream. Appends a newline if the output has no newline at the end. */
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
