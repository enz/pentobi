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

inline int rounded_log_2(float x)
{
#ifdef INT32_MAX // Availability of int32_t is optional
    if (numeric_limits<float>::is_iec559 && numeric_limits<float>::radix == 2
            && numeric_limits<float>::max_exponent == 128)
    {
        union
        {
            int32_t i;
            float f;
        } convert;
        convert.f = x;
        return ((convert.i >> 23) & 0xff) - 127;
    }
#endif
    int exponent;
    frexp(x, &exponent);
    return exponent - 1;
}

inline int rounded_log_2(double x)
{
#ifdef INT64_MAX // Availability of int64_t is optional
    if (numeric_limits<double>::is_iec559 && numeric_limits<double>::radix == 2
            && numeric_limits<double>::max_exponent == 1024)
    {
        union
        {
            int64_t i;
            double f;
        } convert;
        convert.f = x;
        return static_cast<int>(((convert.i >> 52) & 0x7ff) - 1023);
    }
#endif
    int exponent;
    frexp(x, &exponent);
    return exponent - 1;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_MATH_UTIL_H
