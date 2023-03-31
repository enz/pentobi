//-----------------------------------------------------------------------------
/** @file libboardgame_base/CoordPoint.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "CoordPoint.h"

#include <iostream>

namespace libboardgame_base {

//-----------------------------------------------------------------------------

ostream& operator<<(ostream& out, CoordPoint p)
{
    out << '(' << p.x << ',' << p.y << ')';
    return out;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
