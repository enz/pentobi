//----------------------------------------------------------------------------
/** @file libboardgame_util/FastLog.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_FAST_LOG_H
#define LIBBOARDGAME_UTIL_FAST_LOG_H

#include <memory>
#include <cstdint>

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

/** Fast logarithm.
    Computes a fast single precision logarithm based on a lookup table.

    O. Vinyals, G. Friedland, N. Mirghafori: Revisiting a basic function on
    current CPUs: A fast logarithm implementation with adjustable accuracy.
    http://www.icsi.berkeley.edu/pubs/techreports/TR-07-002.pdf */
class FastLog
{
public:
    explicit FastLog(int mantissa_bits);

    FastLog(const FastLog&) = delete;

    FastLog& operator=(const FastLog&) = delete;

    /** Get natural logarithm. */
    float get_log(float val) const;

private:
    union IntFloat
    {
        int32_t m_int;

        float m_float;
    };

    static const int max_mantissa_bits = 23;

    const int m_mantissa_bits_diff;

    unique_ptr<float[]> m_table;
};

inline float FastLog::get_log(float val) const
{
    IntFloat x;
    x.m_float = val;
    int32_t log_2 = ((x.m_int >> max_mantissa_bits) & 255) - 127;
    x.m_int &= 0x7FFFFF;
    x.m_int >>= m_mantissa_bits_diff;
    return ((m_table[x.m_int] + float(log_2)) * 0.69314718f);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_FAST_LOG_H
