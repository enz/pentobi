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

MissingProperty::MissingProperty(const string& message)
    : SgfError("Missing SGF property: " + message)
{
}

MissingProperty::MissingProperty(const string& id, const string& message)
    : SgfError("Missing SGF property '" + id + ": " + message)
{
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
