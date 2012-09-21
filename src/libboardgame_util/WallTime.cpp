//-----------------------------------------------------------------------------
/** @file libboardgame_util/WallTime.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "WallTime.h"

namespace libboardgame_util {

//-----------------------------------------------------------------------------

WallTime::WallTime()
{
#if LIBBOARDGAME_USE_STD_CHRONO
    m_start = std::chrono::system_clock::now();
#else
    m_start = boost::posix_time::microsec_clock::universal_time();
#endif
}

double WallTime::operator()()
{
    // Logically, there is no need to return the time since m_start, we could
    // also use time_since_epoch(), but during debugging it is nicer to
    // deal with smaller numbers.
#if LIBBOARDGAME_USE_STD_CHRONO
    auto diff = std::chrono::system_clock::now() - m_start;
    return std::chrono::duration_cast<duration<double>>(diff).count();
#else
    auto diff = boost::posix_time::microsec_clock::universal_time() - m_start;
    return double(diff.total_nanoseconds()) * 1e-9;
#endif
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
