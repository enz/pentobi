//-----------------------------------------------------------------------------
/** @file libboardgame_util/Abort.h
    Global flag to interrupt move generation or other commands.
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
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
