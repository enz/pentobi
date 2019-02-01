//-----------------------------------------------------------------------------
/** @file libboardgame_util/IntervalChecker.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_INTERVAL_CHECKER_H
#define LIBBOARDGAME_UTIL_INTERVAL_CHECKER_H

#include <functional>
#include "TimeSource.h"

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

/** Reduces regular calls to an expensive function to a given time interval.
    The class assumes that its check() function is called in regular time
    intervals and forwards only every n'th call to the expensive function with
    n being adjusted dynamically to a given time interval. check() returns
    true, if the expensive function was called and returned true in the
    past. */
class IntervalChecker
{
public:
    /** Constructor.
        @param time_source The time source. The lifetime of this
        parameter must exceed the lifetime of the class instance.
        @param time_interval The time interval in seconds
        @param f The expensive function */
    IntervalChecker(TimeSource& time_source, double time_interval,
                    const function<bool()>& f);

    bool operator()();

    /** Disable the dynamic updating of the interval.
        Can be used if the non-reproducability of the time measurement used
        for dynamic updating of the check interval is undesirable.
        @param interval The fixed interval (number of calls) to use for calling
        the expensive function. (Must be greater zero). */
    void set_deterministic(unsigned interval);

protected:
    TimeSource& m_time_source;

private:
    bool m_is_first_check = true;

    bool m_is_deterministic = false;

    bool m_result = false;

    unsigned m_count = 1;

    unsigned m_count_interval = 1;

    double m_time_interval;

    double m_last_time;

    function<bool()> m_function;

    bool check_expensive();
};

inline bool IntervalChecker::operator()()
{
    if (--m_count == 0)
        return check_expensive();
    return m_result;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_INTERVAL_CHECKER_H
