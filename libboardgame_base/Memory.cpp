//-----------------------------------------------------------------------------
/** @file libboardgame_base/Memory.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Memory.h"

#ifdef _WIN32
#include <algorithm>
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace libboardgame_base {

//-----------------------------------------------------------------------------

size_t get_memory()
{
#ifdef _WIN32
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    if (! GlobalMemoryStatusEx(&status))
        return 0;
    return static_cast<size_t>(status.ullTotalPhys);
#elif defined _SC_PHYS_PAGES
    long phys_pages = sysconf(_SC_PHYS_PAGES);
    if (phys_pages < 0)
        return 0;
    long page_size = sysconf(_SC_PAGE_SIZE);
    if (page_size < 0)
        return 0;
    return static_cast<size_t>(phys_pages) * static_cast<size_t>(page_size);
#else
#error "Determining memory size on this platform not (yet) supported"
#endif
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
