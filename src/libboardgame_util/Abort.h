//-----------------------------------------------------------------------------
/** @file libboardgame_util/Abort.h
    Global flag to interrupt move generation or other commands. */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_ABORT_H
#define LIBBOARDGAME_UTIL_ABORT_H

namespace libboardgame_util {

//-----------------------------------------------------------------------------

void set_abort();

void clear_abort();

bool get_abort();

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_ABORT_H
