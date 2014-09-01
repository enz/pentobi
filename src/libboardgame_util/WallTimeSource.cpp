//-----------------------------------------------------------------------------
/** @file libboardgame_util/WallTimeSource.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "WallTimeSource.h"

namespace libboardgame_util {

using namespace std::chrono;

//-----------------------------------------------------------------------------

WallTimeSource::WallTimeSource()
{
    m_start = system_clock::now();
}

double WallTimeSource::operator()()
{
    // Logically, there is no need to return the time since m_start, we could
    // also use time_since_epoch(), but during debugging it is nicer to
    // deal with smaller numbers.
    auto diff = system_clock::now() - m_start;
    return duration_cast<duration<double>>(diff).count();
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
