//-----------------------------------------------------------------------------
/** @file twogtp/Analyze.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Analyze.h"

#include <fstream>
#include <map>
#include "libboardgame_util/FmtSaver.h"
#include "libboardgame_util/Statistics.h"
#include "libboardgame_util/StringUtil.h"

using libboardgame_util::from_string;
using libboardgame_util::split;
using libboardgame_util::trim;
using libboardgame_util::FmtSaver;
using libboardgame_util::Statistics;
using libboardgame_util::StatisticsExt;

//-----------------------------------------------------------------------------

namespace {

void write_result(const Statistics<>& stat)
{
    FmtSaver saver(cout);
    cout << fixed << setprecision(1) << stat.get_mean() * 100 << "+-"
         << stat.get_error() * 100;
}

} // namespace

//-----------------------------------------------------------------------------

void analyze(const string& file)
{
    FmtSaver saver(cout);
    ifstream in(file);
    Statistics<> stat_result;
    map<unsigned, Statistics<>> stat_result_player;
    map<float, unsigned> result_count;
    StatisticsExt<> stat_length;
    StatisticsExt<> stat_cpu_b;
    StatisticsExt<> stat_cpu_w;
    StatisticsExt<> stat_fast_open;
    string line;
    while (getline(in, line))
    {
        line = trim(line);
        if (! line.empty() && line[0] == '#')
            continue;
        auto columns = split(line, '\t');
        if (columns.empty())
            continue;
        float result;
        unsigned length;
        unsigned player;
        float cpu_b;
        float cpu_w;
        unsigned fast_open;
        if (columns.size() != 7
                || ! from_string(columns[1], result)
                || ! from_string(columns[2], length)
                || ! from_string(columns[3], player)
                || ! from_string(columns[4], cpu_b)
                || ! from_string(columns[5], cpu_w)
                || ! from_string(columns[6], fast_open))
            throw runtime_error("invalid format");
        stat_result.add(result);
        stat_result_player[player].add(result);
        ++result_count[result];
        stat_length.add(length);
        stat_cpu_b.add(cpu_b);
        stat_cpu_w.add(cpu_w);
        stat_fast_open.add(fast_open);
    }
    auto count = stat_result.get_count();
    cout << "Gam: " << count;
    if (count == 0)
    {
        cout << '\n';
        return;
    }
    cout << ", Res: ";
    write_result(stat_result);
    cout << " (";
    bool is_first = true;
    for (auto& i : stat_result_player)
    {
        if (! is_first)
            cout << ", ";
        else
            is_first = false;
        cout << i.first << ": ";
        write_result(i.second);
    }
    cout << ")\nResFreq:";
    for (auto& i : result_count)
    {
        cout << ' ' << i.first << "=";
        {
            FmtSaver saver(cout);
            auto fraction = i.second / count;
            cout << fixed << setprecision(1) << fraction * 100
                 << "+-" << sqrt(fraction * (1 - fraction) / count) * 100;
        }
    }
    cout << "\nCpuB: ";
    stat_cpu_b.write(cout, true, 3, false, true);
    cout << "\nCpuW: ";
    stat_cpu_w.write(cout, true, 3, false, true);
    auto cpu_b = stat_cpu_b.get_mean();
    auto cpu_w = stat_cpu_w.get_mean();
    auto err_cpu_b = stat_cpu_b.get_error();
    auto err_cpu_w = stat_cpu_w.get_error();
    cout << "\nCpuB/CpuW: ";
    if (cpu_b > 0 && cpu_w > 0)
        cout << fixed << setprecision(3) << cpu_b / cpu_w << "+-"
             << cpu_b / cpu_w * hypot(err_cpu_b / cpu_b, err_cpu_w / cpu_w);
    else
        cout << "-";
    cout << ", Len: ";
    stat_length.write(cout, true, 1, true, true);
    if (stat_fast_open.get_mean() > 0)
    {
        cout << ", Fast: ";
        stat_fast_open.write(cout, true, 1, true, true);
    }
    cout << '\n';
}

//-----------------------------------------------------------------------------
