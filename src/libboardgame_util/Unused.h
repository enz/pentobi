//-----------------------------------------------------------------------------
/** @file libboardgame_util/Unused.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_UNUSED_H
#define LIBBOARDGAME_UTIL_UNUSED_H

//-----------------------------------------------------------------------------

template<class T> static void LIBBOARDGAME_UNUSED(const T&) { }

#ifdef LIBBOARDGAME_DEBUG
#define LIBBOARDGAME_UNUSED_IF_NOT_DEBUG(x)
#else
#define LIBBOARDGAME_UNUSED_IF_NOT_DEBUG(x) LIBBOARDGAME_UNUSED(x)
#endif

//-----------------------------------------------------------------------------

#endif // LIBBOARDGAME_UTIL_UNUSED_H
