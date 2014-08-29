//-----------------------------------------------------------------------------
/** @file libboardgame_util/Log.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Log.h"

#include <fstream>
#include <iostream>

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

namespace {

ostream* log_stream = &cerr;

/** Unopened file stream serves as null stream. */
ofstream null_stream;

} // namespace

//-----------------------------------------------------------------------------

ostream& get_log()
{
    return *log_stream;
}

void log(const string& s)
{
    if (s.empty())
        *log_stream << '\n';
    else if (s.back() == '\n')
        *log_stream << s;
    else
    {
        string line = s;
        line += '\n';
        *log_stream << line;
    }
}

void set_log(ostream& out)
{
    log_stream = &out;
}

void set_log_null()
{
    set_log(null_stream);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util
