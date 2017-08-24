//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/SgfError.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SgfError.h"

#include <string>

namespace libboardgame_sgf {

//-----------------------------------------------------------------------------

MissingProperty::MissingProperty(const string& id)
    : SgfError("Missing SGF property '" + id + "'")
{
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
