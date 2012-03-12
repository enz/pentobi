//-----------------------------------------------------------------------------
/** @file libboardgame_base/Transform.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Transform.h"

#include <cmath>
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

unsigned int TransfIdentity::get_point_type() const
{
    return 0;
}

unsigned int TransfIdentity::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot90::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.y, -p.x);
}

unsigned int TransfRectRot90::get_point_type() const
{
    return 0;
}

unsigned int TransfRectRot90::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot180::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.x, -p.y);
}

unsigned int TransfRectRot180::get_point_type() const
{
    return 0;
}

unsigned int TransfRectRot180::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot270::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.y, p.x);
}

unsigned int TransfRectRot270::get_point_type() const
{
    return 0;
}

unsigned int TransfRectRot270::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRefl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.x, p.y);
}

unsigned int TransfRectRefl::get_point_type() const
{
    return 0;
}

unsigned int TransfRectRefl::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot90Refl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.y, p.x);
}

unsigned int TransfRectRot90Refl::get_point_type() const
{
    return 0;
}

unsigned int TransfRectRot90Refl::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot180Refl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.x, -p.y);
}

unsigned int TransfRectRot180Refl::get_point_type() const
{
    return 0;
}

unsigned int TransfRectRot180Refl::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot270Refl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.y, -p.x);
}

unsigned int TransfRectRot270Refl::get_point_type() const
{
    return 0;
}

unsigned int TransfRectRot270Refl::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRefl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.x, p.y);
}

unsigned int TransfTrigonRefl::get_point_type() const
{
    return 0;
}

unsigned int TransfTrigonRefl::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRot60::get_transformed(const CoordPoint& p) const
{
    float px = static_cast<float>(p.x);
    float py = static_cast<float>(p.y);
    int x = static_cast<int>(floor(0.5f * px + 1.5f * py));
    int y = static_cast<int>(floor(-0.5f * px + 0.5f * py));
    return CoordPoint(x, y);
}

unsigned int TransfTrigonRot60::get_point_type() const
{
    return 0;
}

unsigned int TransfTrigonRot60::get_new_point_type() const
{
    return 1;
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRot120::get_transformed(const CoordPoint& p) const
{
    float px = static_cast<float>(p.x);
    float py = static_cast<float>(p.y);
    int x = static_cast<int>(floor(-0.5f * px + 1.5f * py));
    int y = static_cast<int>(ceil(-0.5f * px - 0.5f * py));
    return CoordPoint(x, y);
}

unsigned int TransfTrigonRot120::get_point_type() const
{
    return 0;
}

unsigned int TransfTrigonRot120::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRot180::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.x, -p.y);
}

unsigned int TransfTrigonRot180::get_point_type() const
{
    return 0;
}

unsigned int TransfTrigonRot180::get_new_point_type() const
{
    return 1;
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRot240::get_transformed(const CoordPoint& p) const
{
    float px = static_cast<float>(p.x);
    float py = static_cast<float>(p.y);
    int x = static_cast<int>(ceil(-0.5f * px - 1.5f * py));
    int y = static_cast<int>(ceil(0.5f * px - 0.5f * py));
    return CoordPoint(x, y);
}

unsigned int TransfTrigonRot240::get_point_type() const
{
    return 0;
}

unsigned int TransfTrigonRot240::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRot300::get_transformed(const CoordPoint& p) const
{
    float px = static_cast<float>(p.x);
    float py = static_cast<float>(p.y);
    int x = static_cast<int>(ceil(0.5f * px - 1.5f * py));
    int y = static_cast<int>(floor(0.5f * px + 0.5f * py));
    return CoordPoint(x, y);
}

unsigned int TransfTrigonRot300::get_point_type() const
{
    return 0;
}

unsigned int TransfTrigonRot300::get_new_point_type() const
{
    return 1;
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonReflRot60::get_transformed(const CoordPoint& p) const
{
    float px = static_cast<float>(p.x);
    float py = static_cast<float>(p.y);
    int x = static_cast<int>(floor(0.5f * (-px) + 1.5f * py));
    int y = static_cast<int>(floor(-0.5f * (-px) + 0.5f * py));
    return CoordPoint(x, y);
}

unsigned int TransfTrigonReflRot60::get_point_type() const
{
    return 0;
}

unsigned int TransfTrigonReflRot60::get_new_point_type() const
{
    return 1;
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonReflRot120::get_transformed(const CoordPoint& p) const
{
    float px = static_cast<float>(p.x);
    float py = static_cast<float>(p.y);
    int x = static_cast<int>(floor(-0.5f * (-px) + 1.5f * py));
    int y = static_cast<int>(ceil(-0.5f * (-px) - 0.5f * py));
    return CoordPoint(x, y);
}

unsigned int TransfTrigonReflRot120::get_point_type() const
{
    return 0;
}

unsigned int TransfTrigonReflRot120::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonReflRot180::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.x, -p.y);
}

unsigned int TransfTrigonReflRot180::get_point_type() const
{
    return 0;
}

unsigned int TransfTrigonReflRot180::get_new_point_type() const
{
    return 1;
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonReflRot240::get_transformed(const CoordPoint& p) const
{
    float px = static_cast<float>(p.x);
    float py = static_cast<float>(p.y);
    int x = static_cast<int>(ceil(-0.5f * (-px) - 1.5f * py));
    int y = static_cast<int>(ceil(0.5f * (-px) - 0.5f * py));
    return CoordPoint(x, y);
}

unsigned int TransfTrigonReflRot240::get_point_type() const
{
    return 0;
}

unsigned int TransfTrigonReflRot240::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonReflRot300::get_transformed(const CoordPoint& p) const
{
    float px = static_cast<float>(p.x);
    float py = static_cast<float>(p.y);
    int x = static_cast<int>(ceil(0.5f * (-px) - 1.5f * py));
    int y = static_cast<int>(floor(0.5f * (-px) + 0.5f * py));
    return CoordPoint(x, y);
}

unsigned int TransfTrigonReflRot300::get_point_type() const
{
    return 0;
}

unsigned int TransfTrigonReflRot300::get_new_point_type() const
{
    return 1;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
