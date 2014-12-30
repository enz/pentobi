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
#include "Assert.h"

namespace libboardgame_util {

using namespace std;

//----------------------------------------------------------------------------

static_assert(numeric_limits<float>::is_iec559, "");

FastLog::FastLog(unsigned mantissa_bits)
    : m_mantissa_bits_diff(max_mantissa_bits - mantissa_bits),
      m_table(new float[static_cast<size_t>(1) << mantissa_bits])
{
    LIBBOARDGAME_ASSERT(mantissa_bits <= max_mantissa_bits);
    IntFloat x;
    x.m_int = 0x3F800000;
    int incr = (1 << m_mantissa_bits_diff);
    unsigned p = static_cast<unsigned>(pow(2.f, mantissa_bits));
    float inv_log_2 = 1.f / log(2.f);
    for (unsigned i = 0; i < p; ++i)
    {
        m_table[i] = log(x.m_float) * inv_log_2;
        x.m_int += incr;
    }
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
