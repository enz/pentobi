//-----------------------------------------------------------------------------
/** @file libpentobi_base/PointState.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "PointState.h"

#include <iostream>

namespace libpentobi_base {

//-----------------------------------------------------------------------------

ostream& operator<<(ostream& out, PointState s)
{
    if (s.is_color())
        out << s.to_color();
    else
        out << 'E';
    return out;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
