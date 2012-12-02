//-----------------------------------------------------------------------------
/** @file libboardgame_sys/Memory.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Memory.h"

#include <algorithm>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#if HAVE_SYS_SYSCTL_H
#include <sys/sysctl.h>
#endif

namespace libboardgame_sys {

using namespace std;

//-----------------------------------------------------------------------------

size_t get_memory()
{
#if defined WIN32

    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    if (! GlobalMemoryStatusEx(&status))
        return 0;
    size_t total_virtual = static_cast<size_t>(status.ullTotalVirtual);
    size_t total_phys = static_cast<size_t>(status.ullTotalPhys);
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
    if (sysctl(name, 2, &phys_mem, &len, 0, 0) != 0 || len != sizeof(phys_mem))
        return 0;
    else
        return phys_mem;

#else

    return 0;

#endif
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sys
