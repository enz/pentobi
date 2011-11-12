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

Transform::~Transform()
{
}

//-----------------------------------------------------------------------------

CoordPoint TransfIdentity::get_transformed(const CoordPoint& p) const
{
    return p;
}

unsigned int TransfIdentity::get_point_type(unsigned int old_point_type) const
{
    return old_point_type;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot90::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.y, -p.x);
}

unsigned int TransfRectRot90::get_point_type(unsigned int old_point_type) const
{
    LIBBOARDGAME_ASSERT(old_point_type == 0);
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot180::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.x, -p.y);
}

unsigned int TransfRectRot180::get_point_type(unsigned int old_point_type) const
{
    LIBBOARDGAME_ASSERT(old_point_type == 0);
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot270::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.y, p.x);
}

unsigned int TransfRectRot270::get_point_type(unsigned int old_point_type) const
{
    LIBBOARDGAME_ASSERT(old_point_type == 0);
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRefl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.x, p.y);
}

unsigned int TransfRectRefl::get_point_type(unsigned int old_point_type) const
{
    LIBBOARDGAME_ASSERT(old_point_type == 0);
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot90Refl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.y, p.x);
}

unsigned int TransfRectRot90Refl::get_point_type(
                                              unsigned int old_point_type) const
{
    LIBBOARDGAME_ASSERT(old_point_type == 0);
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot180Refl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.x, -p.y);
}

unsigned int TransfRectRot180Refl::get_point_type(
                                              unsigned int old_point_type) const
{
    LIBBOARDGAME_ASSERT(old_point_type == 0);
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot270Refl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.y, -p.x);
}

unsigned int TransfRectRot270Refl::get_point_type(
                                              unsigned int old_point_type) const
{
    LIBBOARDGAME_ASSERT(old_point_type == 0);
    return 0;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
