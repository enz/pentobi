//-----------------------------------------------------------------------------
/** @file libboardgame_util/WallTimeSource.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_WALL_TIME_SOURCE_H
#define LIBBOARDGAME_UTIL_WALL_TIME_SOURCE_H

#include "TimeSource.h"

namespace libboardgame_util {

//-----------------------------------------------------------------------------

/** Wall time.
    @ref libboardgame_doc_threadsafe_after_construction */
class WallTimeSource
    : public TimeSource
{
public:
    double operator()() override;
};
//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_WALL_TIME_SOURCE_H
