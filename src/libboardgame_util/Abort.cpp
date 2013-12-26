//-----------------------------------------------------------------------------
/** @file libboardgame_util/Abort.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
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
    abort.store(false, memory_order_seq_cst);
}

bool get_abort()
{
    return abort.load(memory_order_relaxed);
}

void set_abort()
{
    abort.store(true, memory_order_seq_cst);
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
