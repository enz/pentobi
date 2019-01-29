//-----------------------------------------------------------------------------
/** @file libboardgame_util/CpuTime.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "CpuTime.h"

#ifdef _WIN32
#include <windows.h>
#endif

#if __has_include(<unistd.h>)
#include <unistd.h>
#endif

#if __has_include(<sys/times.h>)
#include <sys/times.h>
#endif

namespace libboardgame_util {

//-----------------------------------------------------------------------------

double cpu_time()
{
#ifdef _WIN32
    FILETIME create;
    FILETIME exit;
    FILETIME sys;
    FILETIME user;
    if (! GetProcessTimes(GetCurrentProcess(), &create, &exit, &sys, &user))
        return -1;
    ULARGE_INTEGER sys_int;
    sys_int.LowPart = sys.dwLowDateTime;
    sys_int.HighPart = sys.dwHighDateTime;
    ULARGE_INTEGER user_int;
    user_int.LowPart = user.dwLowDateTime;
    user_int.HighPart = user.dwHighDateTime;
    return (sys_int.QuadPart + user_int.QuadPart) * 1e-7;
#elif __has_include(<unistd.h>) && __has_include(<sys/times.h>)
    static auto ticks_per_second = double(sysconf(_SC_CLK_TCK));
    struct tms buf;
    if (times(&buf) == clock_t(-1))
        return -1;
    clock_t clock_ticks =
        buf.tms_utime + buf.tms_stime + buf.tms_cutime + buf.tms_cstime;
    return double(clock_ticks) / ticks_per_second;
#else
    return -1;
#endif
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util
