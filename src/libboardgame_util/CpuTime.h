//-----------------------------------------------------------------------------
/** @file libboardgame_util/CpuTime.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_CPU_TIME_H
#define LIBBOARDGAME_UTIL_CPU_TIME_H

#include "TimeSource.h"

namespace libboardgame_util {

//-----------------------------------------------------------------------------

/** CPU time.
    @ref libboardgame_doc_threadsafe_after_construction */
class CpuTime
    : public TimeSource
{
public:
    double operator()();
};
//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_CPU_TIME_H
