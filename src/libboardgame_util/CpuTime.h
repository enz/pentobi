//-----------------------------------------------------------------------------
/** @file libboardgame_util/CpuTime.h
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
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
    double operator()() override;
};
//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_CPU_TIME_H
