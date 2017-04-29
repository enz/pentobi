//-----------------------------------------------------------------------------
/** @file libpentobi_base/Color.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Color.h"

#include <sstream>
#include "libboardgame_util/StringUtil.h"

namespace libpentobi_base {

using libboardgame_util::to_lower;

//-----------------------------------------------------------------------------

ostream& operator<<(ostream& out, Color c)
{
    out << (c.to_int() + 1);
    return out;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
