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

#ifndef LIBBOARDGAME_DISABLE_LOG
extern ostream* _log_stream;
#endif

inline void disable_logging()
{
#ifndef LIBBOARDGAME_DISABLE_LOG
    _log_stream = nullptr;
#endif
}

inline ostream* get_log_stream()
{
#ifndef LIBBOARDGAME_DISABLE_LOG
    return _log_stream;
#else
    return nullptr;
#endif
}

inline void flush_log()
{
#ifndef LIBBOARDGAME_DISABLE_LOG
    if (_log_stream != nullptr)
        _log_stream->flush();
#endif
}

//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_DISABLE_LOG

/** Initializes the logging functionality.
    This is necessary to call on some platforms at the start of the program
    before any calls to log().
    @see LogInitializer */
void _log_init();

/** Closes the logging functionality.
    This is necessary to call on some platforms before the program exits.
    @see LogInitializer */
void _log_close();

/** Helper function needed for log(const Ts&...) */
template<typename T>
void _log_buffered(ostream& buffer, const T& t)
{
    buffer << t;
}

/** Helper function needed for log(const Ts&...) */
template<typename T, typename... Ts>
void _log_buffered(ostream& buffer, const T& first, const Ts&... rest)
{
    buffer << first;
    _log_buffered(buffer, rest...);
}

/** Write a string to the log stream.
    Appends a newline if the output has no newline at the end. */
void _log(const string& s);

/** Write a number of arguments to the log stream.
    Writes to a buffer first so there is only a single write to the log
    stream. Appends a newline if the output has no newline at the end. */
template<typename... Ts>
void _log(const Ts&... args)
{
    if (! _log_stream)
        return;
    ostringstream buffer;
    _log_buffered(buffer, args...);
    _log(buffer.str());
}

#endif //  ! LIBBOARDGAME_DISABLE_LOG

//-----------------------------------------------------------------------------

class LogInitializer
{
public:
    LogInitializer()
    {
#ifndef LIBBOARDGAME_DISABLE_LOG
        _log_init();
#endif
    }

    ~LogInitializer()
    {
#ifndef LIBBOARDGAME_DISABLE_LOG
        _log_close();
#endif
    }
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_DISABLE_LOG
#define LIBBOARDGAME_LOG(...) libboardgame_util::_log(__VA_ARGS__)
#else
#define LIBBOARDGAME_LOG(...) (static_cast<void>(0))
#endif

//-----------------------------------------------------------------------------

#endif // LIBBOARDGAME_UTIL_LOG_H
