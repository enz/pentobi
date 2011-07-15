//-----------------------------------------------------------------------------
/** @file Unused.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_UNUSED_H
#define LIBBOARDGAME_UTIL_UNUSED_H

#include "Assert.h"

//-----------------------------------------------------------------------------

template<class T> static void LIBBOARDGAME_UNUSED(const T&)
{
}

#if LIBBOARDGAME_DEBUG
#define LIBBOARDGAME_UNUSED_IF_NOT_DEBUG(x)
#else
#define LIBBOARDGAME_UNUSED_IF_NOT_DEBUG(x) LIBBOARDGAME_UNUSED(x)
#endif

//-----------------------------------------------------------------------------

#endif // LIBBOARDGAME_UTIL_UNUSED_H
