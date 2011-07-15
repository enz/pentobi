//-----------------------------------------------------------------------------
/** @file IntervalChecker.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_INTERVAL_CHECKER_H
#define LIBBOARDGAME_UTIL_INTERVAL_CHECKER_H

#include <functional>
#include "libboardgame_util/TimeSource.h"

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
        @param time_source (@ref libboardgame_doc_storesref)
        @param time_interval The time interval in seconds
        @param f The expensive function */
    IntervalChecker(TimeSource& time_source, double time_interval,
                    function<bool()> f);

    bool operator()();

protected:
    TimeSource& m_time_source;

private:
    bool m_is_first_check;

    bool m_result;

    unsigned int m_count;

    unsigned int m_count_interval;

    double m_time_interval;

    double m_last_time;

    function<bool()> m_function;

    bool check_expensive();
};

inline bool IntervalChecker::operator()()
{
    if (--m_count == 0)
        return check_expensive();
    else
        return m_result;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_INTERVAL_CHECKER_H
