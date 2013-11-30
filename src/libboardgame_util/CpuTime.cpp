//-----------------------------------------------------------------------------
/** @file libboardgame_util/CpuTime.cpp
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
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
