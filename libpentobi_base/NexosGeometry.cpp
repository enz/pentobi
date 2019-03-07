//-----------------------------------------------------------------------------
/** @file libpentobi_base/NexosGeometry.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "NexosGeometry.h"

#include <memory>

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

NexosGeometry::NexosGeometry()
{
    Geometry::init(25, 25);
}

const NexosGeometry& NexosGeometry::get()
{
    static unique_ptr<NexosGeometry> s_geometry;

    if (! s_geometry)
        s_geometry = make_unique<NexosGeometry>();
    return *s_geometry;
}

auto NexosGeometry::get_adj_coord(
        [[maybe_unused]] int x, [[maybe_unused]] int y) const -> AdjCoordList
{
    return {};
}

auto NexosGeometry::get_diag_coord(int x, int y) const -> DiagCoordList
{
    DiagCoordList l;
    if (get_point_type(x, y) == 1)
    {
        l.push_back({x - 2, y});
        l.push_back({x + 2, y});
        l.push_back({x - 1, y - 1});
        l.push_back({x + 1, y + 1});
        l.push_back({x - 1, y + 1});
        l.push_back({x + 1, y - 1});
    }
    else if (get_point_type(x, y) == 2)
    {
        l.push_back({x, y - 2});
        l.push_back({x, y + 2});
        l.push_back({x - 1, y - 1});
        l.push_back({x + 1, y + 1});
        l.push_back({x - 1, y + 1});
        l.push_back({x + 1, y - 1});
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

