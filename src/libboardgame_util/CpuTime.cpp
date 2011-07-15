//-----------------------------------------------------------------------------
/** @file util/CpuTime.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "CpuTime.h"

#include "libboardgame_sys/CpuTime.h"

namespace libboardgame_util {

using libboardgame_sys::cpu_time;

//-----------------------------------------------------------------------------

double CpuTime::operator()()
{
    return cpu_time();
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
