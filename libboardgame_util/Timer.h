//-----------------------------------------------------------------------------
/** @file libboardgame_util/Timer.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_TIMER_H
#define LIBBOARDGAME_UTIL_TIMER_H

#include "Assert.h"
#include "TimeSource.h"

namespace libboardgame_util {

class Timer
{
public:
    /** Constructor without time source.
        If constructed without time source, the timer cannot be used before
        reset(TimeSource&) was called. */
    Timer() {
#ifdef LIBBOARDGAME_DEBUG
        m_time_source = nullptr;
#endif
    }

    /** Constructor with time_source.
        @param time_source (@ref libboardgame_doc_storesref) */
    explicit Timer(TimeSource& time_source) { reset(time_source); }

    /** Get time since construction or last reset */
    double operator()() const;

    /** Reset timer. */
    void reset();

    /** Set time source and reset timer.
        @param time_source (@ref libboardgame_doc_storesref) */
    void reset(TimeSource& time_source);

private:
    double m_start;

    TimeSource* m_time_source;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_TIMER_H
