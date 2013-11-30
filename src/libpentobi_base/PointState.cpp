//-----------------------------------------------------------------------------
/** @file libpentobi_base/PointState.cpp
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PointState.h"

#include <iostream>

namespace libpentobi_base {

//-----------------------------------------------------------------------------

ostream& operator<<(ostream& out, const PointState& s)
{
    if (s.is_color())
        out << s.to_color();
    else
        out << 'E';
    return out;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
