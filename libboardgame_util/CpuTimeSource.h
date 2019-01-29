//-----------------------------------------------------------------------------
/** @file libboardgame_util/CpuTimeSource.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_CPU_TIME_SOURCE_H
#define LIBBOARDGAME_UTIL_CPU_TIME_SOURCE_H

#include "TimeSource.h"

namespace libboardgame_util {

//-----------------------------------------------------------------------------

/** CPU time.
    @ref libboardgame_doc_threadsafe_after_construction */
class CpuTimeSource
    : public TimeSource
{
public:
    double operator()() override;
};
//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_CPU_TIME_SOURCE_H
