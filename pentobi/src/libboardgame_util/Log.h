//-----------------------------------------------------------------------------
/** @file Log.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_LOG_H
#define LIBBOARDGAME_UTIL_LOG_H

#include <iosfwd>
#include <boost/format.hpp>

namespace libboardgame_util {

using namespace std;
using boost::format;

//-----------------------------------------------------------------------------

ostream& log();

void log(const string& s);

void log(const format& f);

void set_log(ostream& out);

void set_log_null();

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_LOG_H
