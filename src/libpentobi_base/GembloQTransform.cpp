//-----------------------------------------------------------------------------
/** @file libpentobi_base/GembloQTransform.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "GembloQTransform.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

namespace
{

/** Modulo operation that also works for negative integers. */
int mod(int a, int b)
{
    return ((a % b) + b) % b;
}

/** Divide integer by 2 and round down. */
int div2(int a)
{
    return a < 0 ? (a - 1) / 2 : a / 2;
}

} // namespace

//-----------------------------------------------------------------------------

CoordPoint TransfGembloQIdentity::get_transformed(const CoordPoint& p) const
{
    return p;
}

//-----------------------------------------------------------------------------

CoordPoint TransfGembloQRot90::get_transformed(const CoordPoint& p) const
{
    auto y = div2(p.x);
    auto x = -2 * p.y;
    switch (mod(p.x, 4))
    {
    case 0:
    case 3:
        x -= (p.y % 2 != 0);
        break;
    case 1:
    case 2:
        x -= (p.y % 2 == 0);
        break;
    }
    return CoordPoint(x, y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfGembloQRot180::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.x, -p.y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfGembloQRot270::get_transformed(const CoordPoint& p) const
{
    auto y = -div2(p.x);
    auto x = 2 * p.y;
    switch (mod(p.x, 4))
    {
    case 0:
    case 3:
        x += (p.y % 2 != 0);
        break;
    case 1:
    case 2:
        x += (p.y % 2 == 0);
        break;
    }
    return CoordPoint(x, y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfGembloQRefl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(-p.x, p.y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfGembloQRot90Refl::get_transformed(const CoordPoint& p) const
{
    auto y = -div2(p.x);
    auto x = -2 * p.y;
    switch (mod(p.x, 4))
    {
    case 0:
    case 3:
        x -= (p.y % 2 != 0);
        break;
    case 1:
    case 2:
        x -= (p.y % 2 == 0);
        break;
    }
    return CoordPoint(x, y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfGembloQRot180Refl::get_transformed(const CoordPoint& p) const
{
    return CoordPoint(p.x, -p.y);
}

//-----------------------------------------------------------------------------

CoordPoint TransfGembloQRot270Refl::get_transformed(const CoordPoint& p) const
{
    auto y = div2(p.x);
    auto x = 2 * p.y;
    switch (mod(p.x, 4))
    {
    case 0:
    case 3:
        x += (p.y % 2 != 0);
        break;
    case 1:
    case 2:
        x += (p.y % 2 == 0);
        break;
    }
    return CoordPoint(x, y);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
