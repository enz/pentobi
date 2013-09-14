//-----------------------------------------------------------------------------
/** @file libboardgame_util/Math.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Math.h"

namespace libboardgame_util {

//----------------------------------------------------------------------------

double cdf(double x)
{
    // Abramowitz and Stegun: Handbook of Mathematical Functions,
    // formula 7.1.26, http://people.math.sfu.ca/~cbm/aands/page_299.htm
    const double a1 = 0.254829592;
    const double a2 = -0.284496736;
    const double a3 = 1.421413741;
    const double a4 = -1.453152027;
    const double a5 = 1.061405429;
    const double p = 0.3275911;
    const double sign = (x < 0 ? -1 : 1);
    x = fabs(x) / sqrt(2.);
    double t = 1. / (1. + p * x);
    double y =
        1. -
        (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * exp(-x * x);
    return 0.5  * (1. + sign * y);
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
