//-----------------------------------------------------------------------------
/** @file libboardgame_sys/Compiler.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SYS_COMPILER_H
#define LIBBOARDGAME_SYS_COMPILER_H

#include <string>
#if defined __GNUC__
#include <cxxabi.h>
#endif

namespace libboardgame_sys {

using namespace std;

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

template<typename T>
string get_type_name(const T& t)
{
#ifdef __GNUC__
    int status;
    char* name_ptr = abi::__cxa_demangle(typeid(t).name(), 0, 0, &status);
    if (status == 0)
    {
        string result(name_ptr);
        free(name_ptr);
        return result;
    }
#endif
    return typeid(t).name();
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sys

#endif // LIBBOARDGAME_SYS_COMPILER_H
