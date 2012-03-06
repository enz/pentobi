//-----------------------------------------------------------------------------
/** @file WallTime.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "WallTime.h"

namespace libboardgame_util {

using boost::posix_time::microsec_clock;
using boost::posix_time::time_duration;

//-----------------------------------------------------------------------------

WallTime::WallTime()
    : m_start(microsec_clock::universal_time())
{
}

double WallTime::operator()()
{
    time_duration diff = microsec_clock::universal_time() - m_start;
    return double(diff.total_nanoseconds()) * 1e-9;
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
