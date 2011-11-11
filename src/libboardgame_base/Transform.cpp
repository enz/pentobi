//-----------------------------------------------------------------------------
/** @file libboardgame_base/Transform.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Transform.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

Transform::~Transform()
{
}

//-----------------------------------------------------------------------------

CoordPoint TransfIdentity::get_transformed(const CoordPoint& p) const
{
    return p;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRot90::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.y, -p.x);
}

//-----------------------------------------------------------------------------

CoordPoint TransfRot180::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.x, -p.y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfRot270::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.y, p.x);
}

//-----------------------------------------------------------------------------

CoordPoint TransfRefl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.x, p.y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfRot90Refl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.y, p.x);
}

//-----------------------------------------------------------------------------

CoordPoint TransfRot180Refl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.x, -p.y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfRot270Refl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.y, -p.x);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
