//-----------------------------------------------------------------------------
/** @file libboardgame_util/Statistics.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_STATISTICS_H
#define LIBBOARDGAME_UTIL_STATISTICS_H

#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <boost/io/ios_state.hpp>
#include "Assert.h"

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

template<typename FLOAT = double>
class StatisticsBase
{
public:
    StatisticsBase();

    void add(FLOAT val);

    void clear();

    FLOAT get_count() const;

    FLOAT get_mean() const;

    void write(ostream& out, bool fixed = false,
               unsigned int precision = 6) const;

private:
    FLOAT m_count;

    FLOAT m_mean;
};

template<typename FLOAT>
inline StatisticsBase<FLOAT>::StatisticsBase()
{
    clear();
}

template<typename FLOAT>
void StatisticsBase<FLOAT>::add(FLOAT val)
{
    FLOAT count = m_count;
    ++count;
    val -= m_mean;
    m_mean +=  val / count;
    m_count = count;
}

template<typename FLOAT>
inline void StatisticsBase<FLOAT>::clear()
{
    m_count = 0;
    m_mean = 0;
}

template<typename FLOAT>
inline FLOAT StatisticsBase<FLOAT>::get_count() const
{
    return m_count;
}

template<typename FLOAT>
inline FLOAT StatisticsBase<FLOAT>::get_mean() const
{
    LIBBOARDGAME_ASSERT(m_count > 0);
    return m_mean;
}

template<typename FLOAT>
void StatisticsBase<FLOAT>::write(ostream& out, bool fixed,
                                  unsigned int precision) const
{
    if (m_count > 0)
    {
        boost::io::ios_all_saver saver(out);
        if (fixed)
            out << std::fixed;
        out << setprecision(precision) << m_mean;
    }
    else
        out << '-';
}

//----------------------------------------------------------------------------

template<typename FLOAT = double>
class Statistics
{
public:
    Statistics();

    void add(FLOAT val);

    void clear();

    FLOAT get_mean() const;

    FLOAT get_count() const;

    FLOAT get_deviation() const;

    FLOAT get_variance() const;

    void write(ostream& out, bool fixed = false,
               unsigned int precision = 6) const;

private:
    StatisticsBase<FLOAT> m_statistics_base;

    FLOAT m_variance;
};

template<typename FLOAT>
inline Statistics<FLOAT>::Statistics()
{
    clear();
}

template<typename FLOAT>
void Statistics<FLOAT>::add(FLOAT val)
{
    if (get_count() > 0)
    {
        FLOAT count_old = get_count();
        FLOAT mean_old = get_mean();
        m_statistics_base.add(val);
        FLOAT mean = get_mean();
        FLOAT count = get_count();
        m_variance = (count_old * (m_variance + mean_old * mean_old)
                      + val * val) / count  - mean * mean;
    }
    else
    {
        m_statistics_base.add(val);
        m_variance = 0;
    }
}

template<typename FLOAT>
inline void Statistics<FLOAT>::clear()
{
    m_statistics_base.clear();
    m_variance = 0;
}

template<typename FLOAT>
inline FLOAT Statistics<FLOAT>::get_count() const
{
    return m_statistics_base.get_count();
}

template<typename FLOAT>
inline FLOAT Statistics<FLOAT>::get_deviation() const
{
    return sqrt(m_variance);
}

template<typename FLOAT>
inline FLOAT Statistics<FLOAT>::get_mean() const
{
    return m_statistics_base.get_mean();
}

template<typename FLOAT>
inline FLOAT Statistics<FLOAT>::get_variance() const
{
    return m_variance;
}

template<typename FLOAT>
void Statistics<FLOAT>::write(ostream& out, bool fixed, unsigned int precision) const
{
    if (get_count() > 0)
    {
        boost::io::ios_all_saver saver(out);
        if (fixed)
            out << std::fixed;
        out << setprecision(precision) << get_mean() << " dev="
            << get_deviation();
    }
    else
        out << '-';
}

//----------------------------------------------------------------------------

template<typename FLOAT = double>
class StatisticsExt
{
public:
    StatisticsExt();

    void add(FLOAT val);

    void clear();

    FLOAT get_mean() const;

    FLOAT get_count() const;

    FLOAT get_max() const;

    FLOAT get_min() const;

    FLOAT get_deviation() const;

    FLOAT get_variance() const;

    void write(ostream& out, bool fixed = false, unsigned int precision = 6,
               bool integer_values = false) const;

    string to_string(bool fixed = false, unsigned int precision = 6,
                     bool integer_values = false) const;

private:
    Statistics<FLOAT> m_statistics;

    FLOAT m_max;

    FLOAT m_min;
};

template<typename FLOAT>
inline StatisticsExt<FLOAT>::StatisticsExt()
{
    clear();
}

template<typename FLOAT>
void StatisticsExt<FLOAT>::add(FLOAT val)
{
    m_statistics.add(val);
    if (val > m_max)
        m_max = val;
    if (val < m_min)
        m_min = val;
}

template<typename FLOAT>
inline void StatisticsExt<FLOAT>::clear()
{
    m_statistics.clear();
    m_min = numeric_limits<FLOAT>::max();
    m_max = -numeric_limits<FLOAT>::max();
}

template<typename FLOAT>
inline FLOAT StatisticsExt<FLOAT>::get_count() const
{
    return m_statistics.get_count();
}

template<typename FLOAT>
inline FLOAT StatisticsExt<FLOAT>::get_deviation() const
{
    return m_statistics.get_deviation();
}

template<typename FLOAT>
inline FLOAT StatisticsExt<FLOAT>::get_max() const
{
    return m_max;
}

template<typename FLOAT>
inline FLOAT StatisticsExt<FLOAT>::get_mean() const
{
    return m_statistics.get_mean();
}

template<typename FLOAT>
inline FLOAT StatisticsExt<FLOAT>::get_min() const
{
    return m_min;
}

template<typename FLOAT>
inline FLOAT StatisticsExt<FLOAT>::get_variance() const
{
    return m_statistics.get_variance();
}

template<typename FLOAT>
string StatisticsExt<FLOAT>::to_string(bool fixed, unsigned int precision,
                                       bool integer_values) const
{
    ostringstream s;
    write(s, fixed, precision, integer_values);
    return s.str();
}

template<typename FLOAT>
void StatisticsExt<FLOAT>::write(ostream& out, bool fixed, unsigned int precision,
                                 bool integer_values) const
{
    if (get_count() > 0)
    {
        m_statistics.write(out, fixed, precision);
        boost::io::ios_all_saver saver(out);
        if (fixed)
            out << std::fixed;
        if (integer_values)
            out << setprecision(0);
        else
            out << setprecision(precision);
        out << " min=" << m_min << " max=" << m_max;
    }
    else
        out << '-';
}

//----------------------------------------------------------------------------

template<typename FLOAT>
inline ostream& operator<<(ostream& out, const StatisticsExt<FLOAT>& s)
{
    s.write(out);
    return out;
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_STATISTICS_H
