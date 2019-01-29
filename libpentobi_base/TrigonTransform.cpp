//-----------------------------------------------------------------------------
/** @file libpentobi_base/TrigonTransform.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "TrigonTransform.h"

#include <cmath>

namespace libpentobi_base {

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonIdentity::get_transformed(CoordPoint p) const
{
    return p;
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRefl::get_transformed(CoordPoint p) const
{
    return {-p.x, p.y};
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRot60::get_transformed(CoordPoint p) const
{
    auto px = static_cast<float>(p.x);
    auto py = static_cast<float>(p.y);
    auto x = static_cast<int>(std::ceil(0.5f * px - 1.5f * py));
    auto y = static_cast<int>(std::floor(0.5f * px + 0.5f * py));
    return {x, y};
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRot120::get_transformed(CoordPoint p) const
{
    auto px = static_cast<float>(p.x);
    auto py = static_cast<float>(p.y);
    auto x = static_cast<int>(std::ceil(-0.5f * px - 1.5f * py));
    auto y = static_cast<int>(std::ceil(0.5f * px - 0.5f * py));
    return {x, y};
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRot180::get_transformed(CoordPoint p) const
{
    return {-p.x, -p.y};
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRot240::get_transformed(CoordPoint p) const
{
    auto px = static_cast<float>(p.x);
    auto py = static_cast<float>(p.y);
    auto x = static_cast<int>(std::floor(-0.5f * px + 1.5f * py));
    auto y = static_cast<int>(std::ceil(-0.5f * px - 0.5f * py));
    return {x, y};
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonRot300::get_transformed(CoordPoint p) const
{
    auto px = static_cast<float>(p.x);
    auto py = static_cast<float>(p.y);
    auto x = static_cast<int>(std::floor(0.5f * px + 1.5f * py));
    auto y = static_cast<int>(std::floor(-0.5f * px + 0.5f * py));
    return {x, y};
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonReflRot60::get_transformed(CoordPoint p) const
{
    auto px = static_cast<float>(p.x);
    auto py = static_cast<float>(p.y);
    auto x = static_cast<int>(std::ceil(0.5f * (-px) - 1.5f * py));
    auto y = static_cast<int>(std::floor(0.5f * (-px) + 0.5f * py));
    return {x, y};
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonReflRot120::get_transformed(CoordPoint p) const
{
    auto px = static_cast<float>(p.x);
    auto py = static_cast<float>(p.y);
    auto x = static_cast<int>(std::ceil(-0.5f * (-px) - 1.5f * py));
    auto y = static_cast<int>(std::ceil(0.5f * (-px) - 0.5f * py));
    return {x, y};
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonReflRot180::get_transformed(CoordPoint p) const
{
    return {p.x, -p.y};
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonReflRot240::get_transformed(CoordPoint p) const
{
    auto px = static_cast<float>(p.x);
    auto py = static_cast<float>(p.y);
    auto x = static_cast<int>(std::floor(-0.5f * (-px) + 1.5f * py));
    auto y = static_cast<int>(std::ceil(-0.5f * (-px) - 0.5f * py));
    return {x, y};
}

//-----------------------------------------------------------------------------

CoordPoint TransfTrigonReflRot300::get_transformed(CoordPoint p) const
{
    auto px = static_cast<float>(p.x);
    auto py = static_cast<float>(p.y);
    auto x = static_cast<int>(std::floor(0.5f * (-px) + 1.5f * py));
    auto y = static_cast<int>(std::floor(-0.5f * (-px) + 0.5f * py));
    return {x, y};
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
