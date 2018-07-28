//-----------------------------------------------------------------------------
/** @file libboardgame_util/Abort.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Abort.h"

//----------------------------------------------------------------------------

namespace libboardgame_util {

atomic<bool> abort(false);

//----------------------------------------------------------------------------

} // namespace libboardgame_util
