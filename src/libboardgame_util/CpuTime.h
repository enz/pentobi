//-----------------------------------------------------------------------------
/** @file util/CpuTime.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_CPU_TIME_H
#define LIBBOARDGAME_UTIL_CPU_TIME_H

#include "TimeSource.h"

namespace libboardgame_util {

//-----------------------------------------------------------------------------

class CpuTime
    : public TimeSource
{
public:
    double operator()();
};
//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_CPU_TIME_H
