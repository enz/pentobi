//-----------------------------------------------------------------------------
/** @file libboardgame_util/NullElement.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_NULL_ELEMENT_H
#define LIBBOARDGAME_UTIL_NULL_ELEMENT_H

#include "Assert.h"

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

template<class T>
inline void set_null(T& t)
{
    t = T(0);
}

/** Test if object is null.
    The default implementation compares the element against an element
    constructed with 0 as an argument. This function needs to be specialized
    for other types (e.g. using Point::is_null() for
    libboardgame_base::Point) */
template<class T>
inline bool is_null(const T& t)
{
    return t == T(0);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_NULL_ELEMENT_H
