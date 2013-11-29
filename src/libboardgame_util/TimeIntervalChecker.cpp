//-----------------------------------------------------------------------------
/** @file TimeIntervalChecker.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TimeIntervalChecker.h"

namespace libboardgame_util {

//-----------------------------------------------------------------------------

TimeIntervalChecker::TimeIntervalChecker(TimeSource& time_source,
                                         double time_interval,
                                         double max_time)
    : IntervalChecker(time_source, time_interval,
                      bind(&TimeIntervalChecker::check_time, this)),
      m_max_time(max_time),
      m_start_time(m_time_source())
{
}

TimeIntervalChecker::TimeIntervalChecker(TimeSource& time_source,
                                         double max_time)
    : IntervalChecker(time_source, max_time > 1 ? 0.1 : 0.1 * max_time,
                      bind(&TimeIntervalChecker::check_time, this)),
      m_max_time(max_time),
      m_start_time(m_time_source())
{
}

bool TimeIntervalChecker::check_time()
{
    return m_time_source() - m_start_time > m_max_time;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util
