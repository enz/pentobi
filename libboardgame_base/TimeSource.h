//-----------------------------------------------------------------------------
/** @file libboardgame_base/TimeSource.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_TIME_SOURCE_H
#define LIBBOARDGAME_BASE_TIME_SOURCE_H

namespace libboardgame_base {

//-----------------------------------------------------------------------------

/** Abstract time source for measuring thinking times for move generation.
    Typical implementations are wall time, CPU time or mock time sources
    for unit tests. They do not need to provide high resolutions (but should
    support at least 100 ms) and should support maximum times of days (or even
    months). */
class TimeSource
{
public:
    virtual ~TimeSource();

    /** Get the current time in seconds. */
    virtual double operator()() = 0;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_TIME_SOURCE_H
