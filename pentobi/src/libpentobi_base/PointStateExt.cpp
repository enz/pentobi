//-----------------------------------------------------------------------------
/** @file libpentobi_base/PointStateExt.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PointStateExt.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

ostream& operator<<(ostream& out, const PointStateExt& s)
{
    if (! s.is_offboard())
        out << s.to_point_state();
    else
        out << 'O';
    return out;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
