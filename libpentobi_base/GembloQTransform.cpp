//-----------------------------------------------------------------------------
/** @file libpentobi_base/GembloQTransform.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "GembloQTransform.h"

#include "libboardgame_util/MathUtil.h"

namespace libpentobi_base {

using libboardgame_util::mod;

//-----------------------------------------------------------------------------

namespace
{

/** Divide integer by 2 and round down. */
int div2(int a)
{
    return a < 0 ? (a - 1) / 2 : a / 2;
}

} // namespace

//-----------------------------------------------------------------------------

CoordPoint TransfGembloQIdentity::get_transformed(CoordPoint p) const
{
    return p;
}

//-----------------------------------------------------------------------------

CoordPoint TransfGembloQRot90::get_transformed(CoordPoint p) const
{
    auto y = div2(p.x);
    auto x = -2 * p.y;
    switch (mod(p.x, 4))
    {
    case 0:
    case 3:
        x -= static_cast<int>(p.y % 2 != 0);
        break;
    case 1:
    case 2:
        x -= static_cast<int>(p.y % 2 == 0);
        break;
    }
    return {x, y};
}

//-----------------------------------------------------------------------------

CoordPoint TransfGembloQRot180::get_transformed(CoordPoint p) const
{
    return {-p.x, -p.y};
}

//-----------------------------------------------------------------------------

CoordPoint TransfGembloQRot270::get_transformed(CoordPoint p) const
{
    auto y = -div2(p.x);
    auto x = 2 * p.y;
    switch (mod(p.x, 4))
    {
    case 0:
    case 3:
        x += static_cast<int>(p.y % 2 != 0);
        break;
    case 1:
    case 2:
        x += static_cast<int>(p.y % 2 == 0);
        break;
    }
    return {x, y};
}

//-----------------------------------------------------------------------------

CoordPoint TransfGembloQRefl::get_transformed(CoordPoint p) const
{
    return {-p.x, p.y};
}

//-----------------------------------------------------------------------------

CoordPoint TransfGembloQRot90Refl::get_transformed(CoordPoint p) const
{
    auto y = -div2(p.x);
    auto x = -2 * p.y;
    switch (mod(p.x, 4))
    {
    case 0:
    case 3:
        x -= static_cast<int>(p.y % 2 != 0);
        break;
    case 1:
    case 2:
        x -= static_cast<int>(p.y % 2 == 0);
        break;
    }
    return {x, y};
}

//-----------------------------------------------------------------------------

CoordPoint TransfGembloQRot180Refl::get_transformed(CoordPoint p) const
{
    return {p.x, -p.y};
}

//-----------------------------------------------------------------------------

CoordPoint TransfGembloQRot270Refl::get_transformed(CoordPoint p) const
{
    auto y = div2(p.x);
    auto x = 2 * p.y;
    switch (mod(p.x, 4))
    {
    case 0:
    case 3:
        x += static_cast<int>(p.y % 2 != 0);
        break;
    case 1:
    case 2:
        x += static_cast<int>(p.y % 2 == 0);
        break;
    }
    return {x, y};
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
