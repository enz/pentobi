//-----------------------------------------------------------------------------
/** @file libpentobi_base/TrigonGeometry.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "TrigonGeometry.h"

#include <map>
#include <memory>

namespace libpentobi_base {

using namespace std;
using libboardgame_base::CoordPoint;

//-----------------------------------------------------------------------------

TrigonGeometry::TrigonGeometry(unsigned sz)
{
    m_sz = sz;
    Geometry::init(sz * 4 - 1, sz * 2);
}

const TrigonGeometry& TrigonGeometry::get(unsigned sz)
{
    static map<unsigned, shared_ptr<TrigonGeometry>> s_geometry;

    auto pos = s_geometry.find(sz);
    if (pos != s_geometry.end())
        return *pos->second;
    shared_ptr<TrigonGeometry> geometry(new TrigonGeometry(sz));
    return *s_geometry.insert(pair(sz, geometry)).first->second;
}

auto TrigonGeometry::get_adj_coord(int x, int y) const -> AdjCoordList
{
    AdjCoordList l;
    if (get_point_type(x, y) == 0)
    {
        l.push_back(CoordPoint(x - 1, y));
        l.push_back(CoordPoint(x + 1, y));
        l.push_back(CoordPoint(x, y + 1));
    }
    else
    {
        l.push_back(CoordPoint(x, y - 1));
        l.push_back(CoordPoint(x - 1, y));
        l.push_back(CoordPoint(x + 1, y));
    }
    return l;
}

auto TrigonGeometry::get_diag_coord(int x, int y) const -> DiagCoordList
{
    // See Geometry::get_diag_coord() about advantageous ordering of the list
    DiagCoordList l;
    if (get_point_type(x, y) == 0)
    {
        l.push_back(CoordPoint(x - 2, y));
        l.push_back(CoordPoint(x + 2, y));
        l.push_back(CoordPoint(x - 1, y - 1));
        l.push_back(CoordPoint(x + 1, y - 1));
        l.push_back(CoordPoint(x + 1, y + 1));
        l.push_back(CoordPoint(x - 1, y + 1));
        l.push_back(CoordPoint(x, y - 1));
        l.push_back(CoordPoint(x - 2, y + 1));
        l.push_back(CoordPoint(x + 2, y + 1));
    }
    else
    {
        l.push_back(CoordPoint(x - 2, y));
        l.push_back(CoordPoint(x + 2, y));
        l.push_back(CoordPoint(x - 1, y + 1));
        l.push_back(CoordPoint(x + 1, y + 1));
        l.push_back(CoordPoint(x + 1, y - 1));
        l.push_back(CoordPoint(x - 1, y - 1));
        l.push_back(CoordPoint(x, y + 1));
        l.push_back(CoordPoint(x - 2, y - 1));
        l.push_back(CoordPoint(x + 2, y - 1));
    }
    return l;
}

unsigned TrigonGeometry::get_period_x() const
{
    return 2;
}

unsigned TrigonGeometry::get_period_y() const
{
    return 2;
}

unsigned TrigonGeometry::get_point_type(int x, int y) const
{
    if (m_sz % 2 == 0)
    {
        if (x % 2 == 0)
            return y % 2 == 0 ? 1 : 0;
        return y % 2 != 0 ? 1 : 0;
    }
    if (x % 2 != 0)
        return y % 2 == 0 ? 1 : 0;
    return y % 2 != 0 ? 1 : 0;
}

bool TrigonGeometry::init_is_onboard(unsigned x, unsigned y) const
{
    auto width = get_width();
    auto height = get_height();
    unsigned dy = min(y, height - y - 1);
    unsigned min_x = m_sz - dy - 1;
    unsigned max_x = width - min_x - 1;
    return x >= min_x && x <= max_x;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

