//-----------------------------------------------------------------------------
/** @file libboardgame_sys/Memory.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Memory.h"

#ifdef _WIN32
#include <algorithm>
#include <windows.h>
#else
#include <unistd.h>
#endif
// sysctl() is unsupported on Linux with x32 ABI (last checked on Ubuntu 14.10)
#if defined HAVE_SYS_SYSCTL_H && ! (defined __x86_64__ && defined __ILP32__)
#include <sys/sysctl.h>
#endif

namespace libboardgame_sys {

//-----------------------------------------------------------------------------

size_t get_memory()
{
#ifdef _WIN32

    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    if (! GlobalMemoryStatusEx(&status))
        return 0;
    auto total_virtual = static_cast<size_t>(status.ullTotalVirtual);
    auto total_phys = static_cast<size_t>(status.ullTotalPhys);
    return min(total_virtual, total_phys);

#elif defined _SC_PHYS_PAGES

    long phys_pages = sysconf(_SC_PHYS_PAGES);
    if (phys_pages < 0)
        return 0;
    long page_size = sysconf(_SC_PAGE_SIZE);
    if (page_size < 0)
        return 0;
    return static_cast<size_t>(phys_pages) * static_cast<size_t>(page_size);

#elif defined HW_PHYSMEM // Mac OS X

    unsigned int phys_mem;
    size_t len = sizeof(phys_mem);
    int name[2] = { CTL_HW, HW_PHYSMEM };
    if (sysctl(name, 2, &phys_mem, &len, nullptr, 0) != 0
        || len != sizeof(phys_mem))
        return 0;
    else
        return phys_mem;

#else

    return 0;

#endif
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sys
