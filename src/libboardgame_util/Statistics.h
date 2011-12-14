//-----------------------------------------------------------------------------
/** @file libboardgame_util/Statistics.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_STATISTICS_H
#define LIBBOARDGAME_UTIL_STATISTICS_H

#include <cmath>
#include <iosfwd>
#include <limits>
#include <string>
#include "Assert.h"

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

class StatisticsBase
{
public:
    StatisticsBase();

    void add(double val);

    void clear();

    double get_count() const;

    double get_mean() const;

    void write(ostream& out, bool fixed = false,
               unsigned int precision = 6) const;

private:
    double m_count;

    double m_mean;
};

inline StatisticsBase::StatisticsBase()
{
    clear();
}

inline void StatisticsBase::clear()
{
    m_count = 0;
    m_mean = 0;
}

inline double StatisticsBase::get_count() const
{
    return m_count;
}

inline double StatisticsBase::get_mean() const
{
    LIBBOARDGAME_ASSERT(m_count > 0);
    return m_mean;
}

//----------------------------------------------------------------------------

class Statistics
{
public:
    Statistics();

    void add(double val);

    void clear();

    double get_mean() const;

    double get_count() const;

    double get_deviation() const;

    double get_variance() const;

    void write(ostream& out, bool fixed = false,
               unsigned int precision = 6) const;

private:
    StatisticsBase m_statistics_base;

    double m_variance;
};

inline Statistics::Statistics()
{
    clear();
}

inline void Statistics::clear()
{
    m_statistics_base.clear();
    m_variance = 0;
}

inline double Statistics::get_count() const
{
    return m_statistics_base.get_count();
}

inline double Statistics::get_deviation() const
{
    return sqrt(m_variance);
}

inline double Statistics::get_mean() const
{
    return m_statistics_base.get_mean();
}

inline double Statistics::get_variance() const
{
    return m_variance;
}

//----------------------------------------------------------------------------

class StatisticsExt
{
public:
    StatisticsExt();

    void add(double val);

    void clear();

    double get_mean() const;

    double get_count() const;

    double get_max() const;

    double get_min() const;

    double get_deviation() const;

    double get_variance() const;

    void write(ostream& out, bool fixed = false, unsigned int precision = 6,
               bool integer_values = false) const;

    string to_string(bool fixed = false, unsigned int precision = 6,
                     bool integer_values = false) const;

private:
    Statistics m_statistics;

    double m_max;

    double m_min;
};

inline StatisticsExt::StatisticsExt()
{
    clear();
}

inline void StatisticsExt::clear()
{
    m_statistics.clear();
    m_min = numeric_limits<double>::max();
    m_max = -numeric_limits<double>::max();
}

inline double StatisticsExt::get_count() const
{
    return m_statistics.get_count();
}

inline double StatisticsExt::get_deviation() const
{
    return m_statistics.get_deviation();
}

inline double StatisticsExt::get_max() const
{
    return m_max;
}

inline double StatisticsExt::get_mean() const
{
    return m_statistics.get_mean();
}

inline double StatisticsExt::get_min() const
{
    return m_min;
}

inline double StatisticsExt::get_variance() const
{
    return m_statistics.get_variance();
}

//----------------------------------------------------------------------------

inline ostream& operator<<(ostream& out, const StatisticsExt& s)
{
    s.write(out);
    return out;
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_STATISTICS_H
