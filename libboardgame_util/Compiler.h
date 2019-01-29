//-----------------------------------------------------------------------------
/** @file libboardgame_util/Compiler.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_COMPILER_H
#define LIBBOARDGAME_UTIL_COMPILER_H

#include <string>
#include <typeinfo>
#ifdef __GNUC__
#include <cstdlib>
#include <cxxabi.h>
#endif

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

#ifdef __GNUC__
#define LIBBOARDGAME_FORCE_INLINE inline __attribute__((always_inline))
#elif defined _MSC_VER
#define LIBBOARDGAME_FORCE_INLINE inline __forceinline
#else
#define LIBBOARDGAME_FORCE_INLINE inline
#endif

#ifdef __GNUC__
#define LIBBOARDGAME_NOINLINE __attribute__((noinline))
#elif defined _MSC_VER
#define LIBBOARDGAME_NOINLINE __declspec(noinline)
#else
#define LIBBOARDGAME_NOINLINE
#endif

#if defined __GNUC__ && ! defined __ICC &&  ! defined __clang__
#define LIBBOARDGAME_FLATTEN __attribute__((flatten))
#else
#define LIBBOARDGAME_FLATTEN
#endif

template<typename T>
string get_type_name(const T& t)
{
#ifdef __GNUC__
    int status;
    char* name_ptr = abi::__cxa_demangle(typeid(t).name(), nullptr, nullptr,
                                         &status);
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

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_COMPILER_H
