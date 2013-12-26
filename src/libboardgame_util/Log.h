//-----------------------------------------------------------------------------
/** @file libboardgame_util/Log.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_LOG_H
#define LIBBOARDGAME_UTIL_LOG_H

#include <iosfwd>
#include <string>

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

ostream& log();

void log(const string& s);

void set_log(ostream& out);

void set_log_null();

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_LOG_H
