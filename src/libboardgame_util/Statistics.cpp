//-----------------------------------------------------------------------------
/** @file Statistics.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Statistics.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <boost/io/ios_state.hpp>

namespace libboardgame_util {

using boost::io::ios_all_saver;

//-----------------------------------------------------------------------------

void StatisticsBase::add(double val)
{
    double count = m_count;
    ++count;
    val -= m_mean;
    m_mean +=  val / count;
    m_count = count;
}

void StatisticsBase::write(ostream& out, bool fixed,
                           unsigned int precision) const
{
    if (m_count > 0)
    {
        ios_all_saver saver(out);
        if (fixed)
            out << std::fixed;
        out << setprecision(precision) << m_mean;
    }
    else
        out << '-';
}

//----------------------------------------------------------------------------

void Statistics::add(double val)
{
    if (get_count() > 0)
    {
        double count_old = get_count();
        double mean_old = get_mean();
        m_statistics_base.add(val);
        double mean = get_mean();
        double count = get_count();
        m_variance = (count_old * (m_variance + mean_old * mean_old)
                      + val * val) / count  - mean * mean;
    }
    else
    {
        m_statistics_base.add(val);
        m_variance = 0;
    }
}

void Statistics::write(ostream& out, bool fixed, unsigned int precision) const
{
    if (get_count() > 0)
    {
        ios_all_saver saver(out);
        if (fixed)
            out << std::fixed;
        out << setprecision(precision) << get_mean() << " dev="
            << get_deviation();
    }
    else
        out << '-';
}

//-----------------------------------------------------------------------------

void StatisticsExt::add(double val)
{
    m_statistics.add(val);
    if (val > m_max)
        m_max = val;
    if (val < m_min)
        m_min = val;
}

string StatisticsExt::to_string(bool fixed, unsigned int precision,
                                bool integer_values) const
{
    ostringstream s;
    write(s, fixed, precision, integer_values);
    return s.str();
}

void StatisticsExt::write(ostream& out, bool fixed, unsigned int precision,
                          bool integer_values) const
{
    if (get_count() > 0)
    {
        ios_all_saver saver(out);
        if (fixed)
            out << std::fixed;
        out << setprecision(precision);
        m_statistics.write(out);
        if (integer_values)
            out << setprecision(0);
        out << " min=" << m_min << " max=" << m_max;
    }
    else
        out << '-';
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util
