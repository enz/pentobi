//-----------------------------------------------------------------------------
/** @file libboardgame_util/WallTime.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_WALL_TIME_H
#define LIBBOARDGAME_UTIL_WALL_TIME_H

/** Use std::chrono.
    Due to compilation errors in std::chrono with CLang 3.0/libstdc++ (as in
    Ubuntu 12.04), we still use Boost.Date_Time for measuring wall time. */
#define LIBBOARDGAME_USE_STD_CHRONO 0

#if LIBBOARDGAME_USE_STD_CHRONO
#include <chrono>
#else
#include <boost/date_time/posix_time/posix_time.hpp>
#endif
#include "TimeSource.h"

namespace libboardgame_util {

//-----------------------------------------------------------------------------

class WallTime
    : public TimeSource
{
public:
    WallTime();

    double operator()();

private:
#if LIBBOARDGAME_USE_STD_CHRONO
    std::chrono::system_clock::time_point m_start;
#else
    boost::posix_time::ptime m_start;
#endif
};
//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_WALL_TIME_H
