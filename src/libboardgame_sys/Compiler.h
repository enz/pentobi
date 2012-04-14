//-----------------------------------------------------------------------------
/** @file libboardgame_sys/Compiler.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SYS_COMPILER_H
#define LIBBOARDGAME_SYS_COMPILER_H

namespace libboardgame_sys {

//-----------------------------------------------------------------------------

#if defined __GNUC__
#define LIBBOARDGAME_FORCE_INLINE inline __attribute__((always_inline))
#elif defined _MSC_VER
#define LIBBOARDGAME_FORCE_INLINE inline __forceinline
#else
#define LIBBOARDGAME_FORCE_INLINE inline
#endif

#if defined __GNUC__ && ! defined __ICC &&  ! defined __clang__ && \
    (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))
#define LIBBOARDGAME_FLATTEN __attribute__((flatten))
#else
#define LIBBOARDGAME_FLATTEN
#endif

//-----------------------------------------------------------------------------

} // namespace libboardgame_sys

#endif // LIBBOARDGAME_SYS_COMPILER_H
