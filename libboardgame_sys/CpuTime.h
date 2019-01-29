//-----------------------------------------------------------------------------
/** @file libboardgame_sys/CpuTime.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SYS_CPU_TIME_H
#define LIBBOARDGAME_SYS_CPU_TIME_H

namespace libboardgame_sys {

//-----------------------------------------------------------------------------

/** Return the CPU time of the current process.
    @return The CPU time of the current process in seconds or -1, if the
    CPU time cannot be determined. */
double cpu_time();

//-----------------------------------------------------------------------------

} // namespace libboardgame_sys

#endif // LIBBOARDGAME_SYS_CPU_TIME_H
