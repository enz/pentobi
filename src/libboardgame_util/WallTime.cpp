//-----------------------------------------------------------------------------
/** @file libboardgame_util/WallTime.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "WallTime.h"

namespace libboardgame_util {

using namespace std;
using namespace std::chrono;

//-----------------------------------------------------------------------------

WallTime::WallTime()
{
    m_start = system_clock::now();
}

double WallTime::operator()()
{
    // Logically, there is no need to return the time since m_start, we could
    // also use time_since_epoch(), but during debugging it is nicer to
    // deal with smaller numbers.
    auto diff = system_clock::now() - m_start;
    // Note: Explicitely specify all template arguments to class duration in
    // the following line even if duration<double> should be enough because
    // the second parameter defaults to ratio<1>. But if duration<double> is
    // used, the counts are much too high with GCC 4.7.3-1ubuntu1 on
    // Ubuntu 13.04 for unknown reasons.
    return duration_cast<duration<double, ratio<1>>>(diff).count();
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
