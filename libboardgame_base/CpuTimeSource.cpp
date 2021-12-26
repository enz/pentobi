//-----------------------------------------------------------------------------
/** @file libboardgame_base/CpuTimeSource.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "CpuTimeSource.h"

#include "CpuTime.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

double CpuTimeSource::operator()()
{
    return cpu_time();
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
