//-----------------------------------------------------------------------------
/** @file twogtp/OutputFile.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "OutputFile.h"

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include "libboardgame_util/Exception.h"
#include "libboardgame_util/StringUtil.h"

using libboardgame_util::from_string;
using libboardgame_util::split;
using libboardgame_util::trim;
using libboardgame_util::Exception;

//-----------------------------------------------------------------------------

OutputFile::OutputFile(const string& prefix)
    : m_next(0),
      m_prefix(prefix)
{
    m_lock_fd = creat((m_prefix + ".lock").c_str(), 0644);
    if (m_lock_fd == -1)
        throw Exception("OutputFile: could not create lock file");
    if (flock(m_lock_fd, LOCK_EX | LOCK_NB) == -1)
        throw Exception("OutputFile: twogtp already running");
    ifstream in(m_prefix + ".dat");
    if (! in)
        return;
    string line;
    while (getline(in, line))
    {
        line = trim(line);
        if (! line.empty() && line[0] == '#')
            continue;
        auto columns = split(line, '\t');
        if (columns.empty())
            continue;
        unsigned game_number;
        if (! from_string(columns[0], game_number))
            throw Exception("OutputFile: expected game number");
        m_games.insert(make_pair(game_number, line));
    }
    while (m_games.count(m_next) != 0)
        ++m_next;
    if (check_sentinel())
        remove((m_prefix + ".stop").c_str());
}

OutputFile::~OutputFile()
{
    flock(m_lock_fd, LOCK_UN);
    close(m_lock_fd);
    remove((m_prefix + ".lock").c_str());
}

void OutputFile::add_result(unsigned n, float result, unsigned len,
                            unsigned player_black, double cpu_black,
                            double cpu_white, const string& sgf)
{
    lock_guard<mutex> lock(m_mutex);
    ostringstream line;
    line << n << '\t'
         << setprecision(4) << result << '\t'
         << len << '\t'
         << player_black << '\t'
         << setprecision(5) << cpu_black << '\t'
         << cpu_white;
    m_games.insert(make_pair(n, line.str()));
    {
        ofstream out(m_prefix + ".dat");
        out << "# Game\tResult\tLength\tPlayerB\tCpuB\tCpuW\n";
        for (auto& i : m_games)
            out << i.second << '\n';
    }
    {
        ofstream out(m_prefix + ".blksgf", ios::app);
        out << sgf;
    }
}

bool OutputFile::check_sentinel()
{
    return ifstream(m_prefix + ".stop");
}

unsigned OutputFile::get_next()
{
    lock_guard<mutex> lock(m_mutex);
    unsigned n = m_next;
    do
       ++m_next;
    while (m_games.count(m_next) != 0);
    return n;
}

//-----------------------------------------------------------------------------
