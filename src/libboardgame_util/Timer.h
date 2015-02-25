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
        If constructed without time source, the timer cannot be used befor
        reset(TimeSource&) was called. */
    Timer();

    /** Constructor.
        @param time_source (@ref libboardgame_doc_storesref) */
    Timer(TimeSource& time_source);

    double operator()() const;

    void reset();

    void reset(TimeSource& time_source);

private:
    double m_start;

    TimeSource* m_time_source = nullptr;
};

inline double Timer::operator()() const
{
    LIBBOARDGAME_ASSERT(m_time_source);
    return (*m_time_source)() - m_start;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_TIMER_H
