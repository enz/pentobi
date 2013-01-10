//-----------------------------------------------------------------------------
/** @file libboardgame_base/CoordPoint.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "CoordPoint.h"

#include <iostream>

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

ostream& operator<<(ostream& out, const CoordPoint& p)
{
    if (! p.is_null())
        out << '(' << p.x << ',' << p.y << ')';
    else
        out << "NULL";
    return out;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
