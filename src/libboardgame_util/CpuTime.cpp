//-----------------------------------------------------------------------------
/** @file libboardgame_util/CpuTime.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "CpuTime.h"

#include "libboardgame_sys/CpuTime.h"

namespace libboardgame_util {

//-----------------------------------------------------------------------------

double CpuTime::operator()()
{
    return libboardgame_sys::cpu_time();
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
