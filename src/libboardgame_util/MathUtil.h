//-----------------------------------------------------------------------------
/** @file libboardgame_util/MathUtil.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_MATH_UTIL_H
#define LIBBOARDGAME_UTIL_MATH_UTIL_H

#include <cmath>

namespace libboardgame_util {
namespace math_util {

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

} // namespace math_util
} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_MATH_UTIL_H
