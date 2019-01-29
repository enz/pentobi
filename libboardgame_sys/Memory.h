//-----------------------------------------------------------------------------
/** @file libboardgame_sys/Memory.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SYS_MEMORY_H
#define LIBBOARDGAME_SYS_MEMORY_H

#include <cstddef>

namespace libboardgame_sys {

using namespace std;

//-----------------------------------------------------------------------------

/** Get the physical memory available on the system.
    @return The memory in bytes or 0 if the memory could not be determined. */
size_t get_memory();

//-----------------------------------------------------------------------------

} // namespace libboardgame_sys

#endif // LIBBOARDGAME_SYS_MEMORY_H
