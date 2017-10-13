//-----------------------------------------------------------------------------
/** @file libboardgame_util/TimeSource.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TimeSource.h"

namespace libboardgame_util {

//-----------------------------------------------------------------------------

TimeSource::~TimeSource() = default; // Non-inline to avoid GCC -Winline warning

//----------------------------------------------------------------------------

} // namespace libboardgame_util
