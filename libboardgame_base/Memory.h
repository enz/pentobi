//-----------------------------------------------------------------------------
/** @file libboardgame_base/Memory.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_MEMORY_H
#define LIBBOARDGAME_BASE_MEMORY_H

#include <cstddef>

namespace libboardgame_base {

//-----------------------------------------------------------------------------

/** Get the physical memory available on the system.
    @return The memory in bytes or 0 if the memory could not be determined. */
std::size_t get_memory();

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_MEMORY_H
