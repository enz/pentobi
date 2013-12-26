//----------------------------------------------------------------------------
/** @file libboardgame_util/FastLog.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "FastLog.h"

#include <cmath>
#include <limits>

namespace libboardgame_util {

using namespace std;

//----------------------------------------------------------------------------

static_assert(numeric_limits<float>::is_iec559, "");

FastLog::FastLog(int mantissa_bits)
    : m_mantissa_bits_diff(max_mantissa_bits - mantissa_bits),
      m_table(new float[1 << mantissa_bits])
{
    IntFloat x;
    x.m_int = 0x3F800000;
    int incr = (1 << m_mantissa_bits_diff);
    int p = static_cast<int>(pow(2.0f, mantissa_bits));
    float inv_log_2 = 1.f / log(2.f);
    for (int i = 0; i < p; ++i)
    {
        m_table[i] = log(x.m_float) * inv_log_2;
        x.m_int += incr;
    }
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
