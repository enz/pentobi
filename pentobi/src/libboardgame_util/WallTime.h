//-----------------------------------------------------------------------------
/** @file WallTime.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_WALL_TIME_H
#define LIBBOARDGAME_UTIL_WALL_TIME_H

#include <boost/date_time/posix_time/posix_time.hpp>
#include "TimeSource.h"

namespace libboardgame_util {

using boost::posix_time::ptime;

//-----------------------------------------------------------------------------

class WallTime
    : public TimeSource
{
public:
    WallTime();

    double operator()();

private:
    ptime m_start;
};
//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_WALL_TIME_H
