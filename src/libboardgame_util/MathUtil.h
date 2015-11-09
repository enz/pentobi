//----------------------------------------------------------------------------
/** @file libboardgame_util/MathUtil.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_MATH_UTIL_H
#define LIBBOARDGAME_UTIL_MATH_UTIL_H

#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

/** Fast approximation of exp(x).
    The error is less than 15% for abs(x) \< 10 */
template<typename T>
inline T fast_exp(T x)
{
    x = static_cast<T>(1) + x / static_cast<T>(256);
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    return x;
}

template<typename F>
inline int rounded_log_2(F x)
{
    static_assert(is_floating_point<F>::value, "");
    // Optimized implementation for IEEE-754
    if (numeric_limits<F>::is_iec559 && sizeof(F) == 4)
    {
        union
        {
            int32_t i;
            float f;
        } convert;
        convert.f = x;
        // The last added 1 is to be compatible with frexp()
        return ((convert.i >> 23) & 0xff) - 127 + 1;
    }
    else
    {
        // Portable fallback. Slower because frexp is not inline and computes
        // the significant, which we don't need.
        int exponent;
        frexp(x, &exponent);
        return exponent;
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_MATH_UTIL_H
