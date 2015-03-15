//-----------------------------------------------------------------------------
/** @file libboardgame_base/RectTransform.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RectTransform.h"

#include <cmath>
#include "libboardgame_util/Assert.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

CoordPoint TransfIdentity::get_transformed(const CoordPoint& p) const
{
    return p;
}

unsigned TransfIdentity::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot90::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.y, p.x);
}

unsigned TransfRectRot90::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot180::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.x, -p.y);
}

unsigned TransfRectRot180::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot270::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.y, -p.x);
}

unsigned TransfRectRot270::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRefl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.x, p.y);
}

unsigned TransfRectRefl::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot90Refl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.y, -p.x);
}

unsigned TransfRectRot90Refl::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot180Refl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.x, -p.y);
}

unsigned TransfRectRot180Refl::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot270Refl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.y, p.x);
}

unsigned TransfRectRot270Refl::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
