//-----------------------------------------------------------------------------
/** @file libboardgame_base/Compiler.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_COMPILER_H
#define LIBBOARDGAME_BASE_COMPILER_H

#include <string>
#include <typeinfo>
#if defined __GNUC__ && __has_include(<cxxabi.h>)
#include <cstdlib>
#include <cxxabi.h>
#endif

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

template<typename T>
string get_type_name(const T& t)
{
#if defined __GNUC__ && __has_include(<cxxabi.h>)
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

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_COMPILER_H
