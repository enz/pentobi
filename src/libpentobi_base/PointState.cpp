//-----------------------------------------------------------------------------
/** @file libpentobi_base/PointState.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PointState.h"

namespace libpentobi_base {

using namespace std;

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
