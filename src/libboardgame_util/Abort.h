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

extern atomic<bool> pause;

inline void clear_pause() { pause = false; }

inline void clear_abort()
{
    abort = false;
    clear_pause();
}

inline bool get_abort() { return abort; }

inline void set_abort() { abort = true; }

inline bool get_pause() { return pause; }

inline void set_pause() { pause = true; }

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_ABORT_H
