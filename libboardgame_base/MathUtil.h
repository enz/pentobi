//----------------------------------------------------------------------------
/** @file libboardgame_base/MathUtil.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_MATH_UTIL_H
#define LIBBOARDGAME_BASE_MATH_UTIL_H

namespace libboardgame_base {

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

/** Modulus operation with always positive result. */
inline int mod(int a, int b)
{
    return ((a % b) + b) % b;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_MATH_UTIL_H
