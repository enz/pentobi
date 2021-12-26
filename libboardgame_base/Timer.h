//-----------------------------------------------------------------------------
/** @file libboardgame_base/Timer.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_TIMER_H
#define LIBBOARDGAME_BASE_TIMER_H

namespace libboardgame_base {

class TimeSource;

//-----------------------------------------------------------------------------

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
        @param time_source The time source. The lifetime of this
        parameter must exceed the lifetime of the class instance. */
    explicit Timer(TimeSource& time_source) { reset(time_source); }

    /** Get time since construction or last reset */
    double operator()() const;

    /** Reset timer. */
    void reset();

    /** Set time source and reset timer.
        @param time_source The time source. The lifetime of this
        parameter must exceed the lifetime of the class instance. */
    void reset(TimeSource& time_source);

private:
    double m_start;

    TimeSource* m_time_source;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_TIMER_H
