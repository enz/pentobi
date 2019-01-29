//-----------------------------------------------------------------------------
/** @file libboardgame_util/WallTimeSource.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "WallTimeSource.h"

#include <chrono>

namespace libboardgame_util {

using namespace std::chrono;

//-----------------------------------------------------------------------------

double WallTimeSource::operator()()
{
    auto t = system_clock::now().time_since_epoch();
    return duration_cast<duration<double>>(t).count();
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
