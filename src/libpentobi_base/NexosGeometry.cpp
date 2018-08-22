//-----------------------------------------------------------------------------
/** @file libpentobi_base/NexosGeometry.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "NexosGeometry.h"

#include <memory>
#include "libboardgame_util/Unused.h"

namespace libpentobi_base {

using libboardgame_base::CoordPoint;

//-----------------------------------------------------------------------------

NexosGeometry::NexosGeometry()
{
    Geometry::init(25, 25);
}

const NexosGeometry& NexosGeometry::get()
{
    static unique_ptr<NexosGeometry> s_geometry;

    if (! s_geometry)
        s_geometry.reset(new NexosGeometry());
    return *s_geometry;
}

auto NexosGeometry::get_adj_coord(int x, int y) const -> AdjCoordList
{
    LIBBOARDGAME_UNUSED(x);
    LIBBOARDGAME_UNUSED(y);
    return AdjCoordList();
}

auto NexosGeometry::get_diag_coord(int x, int y) const -> DiagCoordList
{
    DiagCoordList l;
    if (get_point_type(x, y) == 1)
    {
        l.push_back(CoordPoint(x - 2, y));
        l.push_back(CoordPoint(x + 2, y));
        l.push_back(CoordPoint(x - 1, y - 1));
        l.push_back(CoordPoint(x + 1, y + 1));
        l.push_back(CoordPoint(x - 1, y + 1));
        l.push_back(CoordPoint(x + 1, y - 1));
    }
    else if (get_point_type(x, y) == 2)
    {
        l.push_back(CoordPoint(x, y - 2));
        l.push_back(CoordPoint(x, y + 2));
        l.push_back(CoordPoint(x - 1, y - 1));
        l.push_back(CoordPoint(x + 1, y + 1));
        l.push_back(CoordPoint(x - 1, y + 1));
        l.push_back(CoordPoint(x + 1, y - 1));
    }
    return l;
}

unsigned NexosGeometry::get_period_x() const
{
    return 2;
}

unsigned NexosGeometry::get_period_y() const
{
    return 2;
}

unsigned NexosGeometry::get_point_type(int x, int y) const
{
    if (x % 2 == 0)
        return y % 2 == 0 ? 0 : 2;
    return y % 2 == 0 ? 1 : 3;
}

bool NexosGeometry::init_is_onboard(unsigned x, unsigned y) const
{
    return x < get_width() && y < get_height()
            && get_point_type(static_cast<int>(x), static_cast<int>(y)) != 3;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

