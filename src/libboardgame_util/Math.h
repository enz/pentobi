//-----------------------------------------------------------------------------
/** @file libboardgame_util/Math.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_MATH_H
#define LIBBOARDGAME_UTIL_MATH_H

#include <cmath>

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

/** Same as std::round(), which is not available in MSVC10. */
inline float round(float val)
{
    return floor(val + 0.5f);
}

/** Same as std::round(), which is not available in MSVC10. */
inline double round(double val)
{
    return floor(val + 0.5);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_MATH_H
