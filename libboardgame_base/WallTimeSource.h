//-----------------------------------------------------------------------------
/** @file libboardgame_base/WallTimeSource.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_WALL_TIME_SOURCE_H
#define LIBBOARDGAME_BASE_WALL_TIME_SOURCE_H

#include "TimeSource.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

/** Wall time. */
class WallTimeSource
    : public TimeSource
{
public:
    double operator()() override;
};
//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_WALL_TIME_SOURCE_H
