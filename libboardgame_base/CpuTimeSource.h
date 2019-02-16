//-----------------------------------------------------------------------------
/** @file libboardgame_base/CpuTimeSource.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_CPU_TIME_SOURCE_H
#define LIBBOARDGAME_BASE_CPU_TIME_SOURCE_H

#include "TimeSource.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

/** CPU time. */
class CpuTimeSource
    : public TimeSource
{
public:
    double operator()() override;
};
//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_CPU_TIME_SOURCE_H
