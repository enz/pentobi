//-----------------------------------------------------------------------------
/** @file libpentobi_base/NexosGeometry.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "NexosGeometry.h"

#include "libboardgame_util/Unused.h"

namespace libpentobi_base {

using libboardgame_base::CoordPoint;

//-----------------------------------------------------------------------------

map<unsigned, shared_ptr<NexosGeometry>> NexosGeometry::s_geometry;

NexosGeometry::NexosGeometry(unsigned sz)
{
    Geometry::init(sz * 2 - 1, sz * 2 - 1);
}

const NexosGeometry& NexosGeometry::get(unsigned sz)
{
    auto pos = s_geometry.find(sz);
    if (pos != s_geometry.end())
        return *pos->second;
    shared_ptr<NexosGeometry> geometry(new NexosGeometry(sz));
    return *s_geometry.insert(make_pair(sz, geometry)).first->second;
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
    return x < get_width() && y < get_height() && get_point_type(x, y) != 3;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

