//-----------------------------------------------------------------------------
/** @file libboardgame_util/TimeSource.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_TIME_SOURCE_H
#define LIBBOARDGAME_UTIL_TIME_SOURCE_H

namespace libboardgame_util {

//-----------------------------------------------------------------------------

/** Abstract time source for measuring thinking times for move generation.
    Typical implementations are wall time, CPU time or mock time sources
    for unit tests. They do not need to provide high resolutions (but should
    support at least 100 ms) and should support maximum times of days (or even
    months).
    @ref libboardgame_doc_threadsafe_after_construction */
class TimeSource
{
public:
    virtual ~TimeSource();

    /** Get the current time in seconds. */
    virtual double operator()() = 0;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_TIME_SOURCE_H
