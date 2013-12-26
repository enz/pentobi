//-----------------------------------------------------------------------------
/** @file libboardgame_base/Transform.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
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

unsigned TransfIdentity::get_point_type() const
{
    return 0;
}

unsigned TransfIdentity::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot90::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.y, -p.x);
}

unsigned TransfRectRot90::get_point_type() const
{
    return 0;
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

unsigned TransfRectRot180::get_point_type() const
{
    return 0;
}

unsigned TransfRectRot180::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot270::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.y, p.x);
}

unsigned TransfRectRot270::get_point_type() const
{
    return 0;
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

unsigned TransfRectRefl::get_point_type() const
{
    return 0;
}

unsigned TransfRectRefl::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot90Refl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.y, p.x);
}

unsigned TransfRectRot90Refl::get_point_type() const
{
    return 0;
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

unsigned TransfRectRot180Refl::get_point_type() const
{
    return 0;
}

unsigned TransfRectRot180Refl::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfRectRot270Refl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.y, -p.x);
}

unsigned TransfRectRot270Refl::get_point_type() const
{
    return 0;
}

unsigned TransfRectRot270Refl::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRefl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.x, p.y);
}

unsigned TransfTrigonRefl::get_point_type() const
{
    return 0;
}

unsigned TransfTrigonRefl::get_new_point_type() const
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

unsigned TransfTrigonRot60::get_point_type() const
{
    return 0;
}

unsigned TransfTrigonRot60::get_new_point_type() const
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

unsigned TransfTrigonRot120::get_point_type() const
{
    return 0;
}

unsigned TransfTrigonRot120::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRot180::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.x, -p.y);
}

unsigned TransfTrigonRot180::get_point_type() const
{
    return 0;
}

unsigned TransfTrigonRot180::get_new_point_type() const
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

unsigned TransfTrigonRot240::get_point_type() const
{
    return 0;
}

unsigned TransfTrigonRot240::get_new_point_type() const
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

unsigned TransfTrigonRot300::get_point_type() const
{
    return 0;
}

unsigned TransfTrigonRot300::get_new_point_type() const
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

unsigned TransfTrigonReflRot60::get_point_type() const
{
    return 0;
}

unsigned TransfTrigonReflRot60::get_new_point_type() const
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

unsigned TransfTrigonReflRot120::get_point_type() const
{
    return 0;
}

unsigned TransfTrigonReflRot120::get_new_point_type() const
{
    return 0;
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonReflRot180::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.x, -p.y);
}

unsigned TransfTrigonReflRot180::get_point_type() const
{
    return 0;
}

unsigned TransfTrigonReflRot180::get_new_point_type() const
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

unsigned TransfTrigonReflRot240::get_point_type() const
{
    return 0;
}

unsigned TransfTrigonReflRot240::get_new_point_type() const
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

unsigned TransfTrigonReflRot300::get_point_type() const
{
    return 0;
}

unsigned TransfTrigonReflRot300::get_new_point_type() const
{
    return 1;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
