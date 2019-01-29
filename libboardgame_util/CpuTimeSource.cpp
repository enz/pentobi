//-----------------------------------------------------------------------------
/** @file libboardgame_util/CpuTimeSource.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "CpuTimeSource.h"

#include "libboardgame_util/CpuTime.h"

namespace libboardgame_util {

//-----------------------------------------------------------------------------

double CpuTimeSource::operator()()
{
    return cpu_time();
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
