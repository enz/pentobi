//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/Failure.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_GTP_FAILURE_H
#define LIBBOARDGAME_GTP_FAILURE_H

#include <stdexcept>

namespace libboardgame_gtp {

//-----------------------------------------------------------------------------

/** GTP failure.
    Command handlers generate a GTP error response by throwing an instance
    of Failure. */
class Failure
    : public std::runtime_error
{
    using runtime_error::runtime_error;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp

#endif // LIBBOARDGAME_GTP_FAILURE_H
