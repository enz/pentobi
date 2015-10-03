//-----------------------------------------------------------------------------
/** @file libpentobi_base/TrigonTransform.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TrigonTransform.h"

#include <cmath>

namespace libpentobi_base {

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonIdentity::get_transformed(const CoordPoint& p) const
{
    return p;
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRefl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.x, p.y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRot60::get_transformed(const CoordPoint& p) const
{
    float px = static_cast<float>(p.x);
    float py = static_cast<float>(p.y);
    int x = static_cast<int>(ceil(0.5f * px - 1.5f * py));
    int y = static_cast<int>(floor(0.5f * px + 0.5f * py));
    return CoordPoint(x, y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRot120::get_transformed(const CoordPoint& p) const
{
    float px = static_cast<float>(p.x);
    float py = static_cast<float>(p.y);
    int x = static_cast<int>(ceil(-0.5f * px - 1.5f * py));
    int y = static_cast<int>(ceil(0.5f * px - 0.5f * py));
    return CoordPoint(x, y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRot180::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.x, -p.y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRot240::get_transformed(const CoordPoint& p) const
{
    float px = static_cast<float>(p.x);
    float py = static_cast<float>(p.y);
    int x = static_cast<int>(floor(-0.5f * px + 1.5f * py));
    int y = static_cast<int>(ceil(-0.5f * px - 0.5f * py));
    return CoordPoint(x, y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRot300::get_transformed(const CoordPoint& p) const
{
    float px = static_cast<float>(p.x);
    float py = static_cast<float>(p.y);
    int x = static_cast<int>(floor(0.5f * px + 1.5f * py));
    int y = static_cast<int>(floor(-0.5f * px + 0.5f * py));
    return CoordPoint(x, y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonReflRot60::get_transformed(const CoordPoint& p) const
{
    float px = static_cast<float>(p.x);
    float py = static_cast<float>(p.y);
    int x = static_cast<int>(ceil(0.5f * (-px) - 1.5f * py));
    int y = static_cast<int>(floor(0.5f * (-px) + 0.5f * py));
    return CoordPoint(x, y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonReflRot120::get_transformed(const CoordPoint& p) const
{
    float px = static_cast<float>(p.x);
    float py = static_cast<float>(p.y);
    int x = static_cast<int>(ceil(-0.5f * (-px) - 1.5f * py));
    int y = static_cast<int>(ceil(0.5f * (-px) - 0.5f * py));
    return CoordPoint(x, y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonReflRot180::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.x, -p.y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonReflRot240::get_transformed(const CoordPoint& p) const
{
    float px = static_cast<float>(p.x);
    float py = static_cast<float>(p.y);
    int x = static_cast<int>(floor(-0.5f * (-px) + 1.5f * py));
    int y = static_cast<int>(ceil(-0.5f * (-px) - 0.5f * py));
    return CoordPoint(x, y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonReflRot300::get_transformed(const CoordPoint& p) const
{
    float px = static_cast<float>(p.x);
    float py = static_cast<float>(p.y);
    int x = static_cast<int>(floor(0.5f * (-px) + 1.5f * py));
    int y = static_cast<int>(floor(-0.5f * (-px) + 0.5f * py));
    return CoordPoint(x, y);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
