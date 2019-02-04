//-----------------------------------------------------------------------------
/** @file twogtp/Output.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Output.h"

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include "libboardgame_util/StringUtil.h"

using libboardgame_util::from_string;
using libboardgame_util::split;
using libboardgame_util::trim;

//-----------------------------------------------------------------------------

Output::Output(Variant variant, const string& prefix, bool create_tree)
    : m_create_tree(create_tree),
      m_prefix(prefix),
      m_output_tree(variant)
{
    m_lock_fd = creat((prefix + ".lock").c_str(), 0644);
    if (m_lock_fd == -1)
        throw runtime_error("Output: could not create lock file");
    if (flock(m_lock_fd, LOCK_EX | LOCK_NB) == -1)
        throw runtime_error("Output: twogtp already running");
    m_timer.reset(m_time_source);
    ifstream in(prefix + ".dat");
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
            throw runtime_error("Output: expected game number");
        m_games.insert({game_number, line});
    }
    while (m_games.count(m_next) != 0)
        ++m_next;
    if (check_sentinel())
        remove((prefix + ".stop").c_str());
    if (m_create_tree && m_next > 0)
        m_output_tree.load(prefix + "-tree.blksgf");
}

Output::~Output()
{
    save();
    flock(m_lock_fd, LOCK_UN);
    close(m_lock_fd);
    remove((m_prefix + ".lock").c_str());
}

void Output::add_result(unsigned n, float result, const Board& bd,
                        unsigned player_black, double cpu_black,
                        double cpu_white, const string& sgf,
                        const array<bool, Board::max_moves>& is_real_move)
{
    {
        lock_guard lock(m_mutex);
        unsigned nu_fast_open = 0;
        for (unsigned i = 0; i < bd.get_nu_moves(); ++i)
            if (! is_real_move[i])
                ++nu_fast_open;
        ostringstream line;
        line << n << '\t'
             << setprecision(4) << result << '\t'
             << bd.get_nu_moves() << '\t'
             << player_black << '\t'
             << setprecision(5) << cpu_black << '\t'
             << cpu_white << '\t'
             << nu_fast_open;
        m_games.insert({n, line.str()});
        m_sgf_buffer << sgf;
        if (m_create_tree)
            m_output_tree.add_game(bd, player_black, result, is_real_move);
    }
    if (m_timer() > m_save_interval)
    {
        save();
        m_timer.reset();
    }
}

bool Output::check_sentinel()
{
    return ! ifstream(m_prefix + ".stop").fail();
}

bool Output::generate_fast_open_move(bool is_player_black, const Board& bd,
                                     Color to_play, Move& mv)
{
    lock_guard lock(m_mutex);
    m_output_tree.generate_move(is_player_black, bd, to_play, mv);
    return ! mv.is_null();
}

unsigned Output::get_next()
{
    lock_guard lock(m_mutex);
    unsigned n = m_next;
    do
       ++m_next;
    while (m_games.count(m_next) != 0);
    return n;
}

void Output::save()
{
    lock_guard lock(m_mutex);
    {
        ofstream out(m_prefix + ".dat");
        out << "# Game\tResult\tLength\tPlayerB\tCpuB\tCpuW\tFast\n";
        for (auto& i : m_games)
            out << i.second << '\n';
    }
    {
        ofstream out(m_prefix + ".blksgf", ios::app);
        out << m_sgf_buffer.str();
        m_sgf_buffer.str("");
    }
    if (m_create_tree)
        m_output_tree.save(m_prefix + "-tree.blksgf");
}

//-----------------------------------------------------------------------------
