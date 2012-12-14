//-----------------------------------------------------------------------------
/** @file libboardgame_util/Abort.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Abort.h"

#include <atomic>

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

namespace {

atomic<bool> abort(false);

} // namespace

//----------------------------------------------------------------------------

void clear_abort()
{
    abort.store(false, memory_order_release);
}

bool get_abort()
{
    return abort.load(memory_order_consume);
}

void set_abort()
{
    abort.store(true, memory_order_release);
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
