//-----------------------------------------------------------------------------
/** @file twogtp/OutputFile.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef TWOGTP_OUTPUT_FILE_H
#define TWOGTP_OUTPUT_FILE_H

#include <string>
#include <map>
#include <mutex>

using namespace std;

//-----------------------------------------------------------------------------

class OutputFile
{
public:
    OutputFile(const string& prefix);

    ~OutputFile();

    void add_result(unsigned n, float result, unsigned len,
                    unsigned player_black, double cpu_black, double cpu_white,
                    const string& sgf);

    unsigned get_next();

    bool check_sentinel();

private:
    unsigned m_next;

    int m_lock_fd;

    string m_prefix;

    mutex m_mutex;

    map<unsigned, string> m_games;
};

//-----------------------------------------------------------------------------

#endif // TWOGTP_OUTPUT_FILE_H
