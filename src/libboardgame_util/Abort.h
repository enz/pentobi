//-----------------------------------------------------------------------------
/** @file libboardgame_util/Abort.h
    Global flag to interrupt move generation or other commands.
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_ABORT_H
#define LIBBOARDGAME_UTIL_ABORT_H

#include <atomic>

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

extern atomic<bool> abort;

inline void clear_abort()
{
    abort.store(false, memory_order_seq_cst);
}

inline bool get_abort()
{
    return abort.load(memory_order_relaxed);
}

inline void set_abort()
{
    abort.store(true, memory_order_seq_cst);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_ABORT_H
