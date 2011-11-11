//-----------------------------------------------------------------------------
/** @file libboardgame_base/Transform.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Transform.h"
#include "libboardgame_util/Assert.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

CoordPoint Transform::get_transformed(const CoordPoint& p) const
{
    switch (m_type)
    {
    case identity:
        return p;
    case rotate_90:
        return CoordPoint(p.y, -p.x);
    case rotate_180:
        return CoordPoint(-p.x, -p.y);
    case rotate_270:
        return CoordPoint(-p.y, p.x);
    case mirror:
        return CoordPoint(-p.x, p.y);
    case rotate_90_mirror:
        return CoordPoint(p.y, p.x);
    case rotate_180_mirror:
        return CoordPoint(p.x, -p.y);
    default:
        LIBBOARDGAME_ASSERT(m_type == rotate_270_mirror);
        return CoordPoint(-p.y, -p.x);
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
