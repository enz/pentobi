//-----------------------------------------------------------------------------
/** @file libboardgame_util/Abort.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Abort.h"

/** Use std::atomic.
    Not supported by GCC 4.4 in the current stable Debian. Switch to new code
    when Debian 7 is released. Using volatile instead of atomic is not
    portable. */
#define LIBBOARDGAME_USE_STD_ATOMIC 0

#if LIBBOARDGAME_USE_STD_ATOMIC
#include <atomic>
#endif

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

namespace {

#if LIBBOARDGAME_USE_STD_ATOMIC
atomic<bool> abort(false);
#else
volatile bool abort = false;
#endif

} // namespace

//----------------------------------------------------------------------------

void clear_abort()
{
#if LIBBOARDGAME_USE_STD_ATOMIC
    abort.store(false, memory_order_relaxed);
#else
    abort = false;
#endif
}

bool get_abort()
{
#if LIBBOARDGAME_USE_STD_ATOMIC
    return abort.load(memory_order_relaxed);
#else
    return abort;
#endif
}

void set_abort()
{
#if LIBBOARDGAME_USE_STD_ATOMIC
    abort.store(true, memory_order_relaxed);
#else
    abort = true;
#endif
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
