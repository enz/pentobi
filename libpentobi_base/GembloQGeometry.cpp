//-----------------------------------------------------------------------------
/** @file libpentobi_base/GembloQGeometry.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "GembloQGeometry.h"

#include <map>
#include <memory>
#include "libboardgame_base/MathUtil.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_base::mod;
using libboardgame_base::CoordPoint;

//-----------------------------------------------------------------------------

GembloQGeometry::GembloQGeometry(unsigned nu_players)
{
    unsigned height;
    if (nu_players == 2)
    {
        height = 22;
        m_edge = 4;
    }
    else if (nu_players == 3)
    {
        height = 26;
        m_edge = 6;
    }
    else
    {
        LIBBOARDGAME_ASSERT(nu_players == 4);
        height = 28;
        m_edge = 13;
    }
    Geometry::init(2 * height, height);
}

const GembloQGeometry& GembloQGeometry::get(unsigned nu_players)
{
    static map<unsigned, shared_ptr<GembloQGeometry>> s_geometry;

    auto pos = s_geometry.find(nu_players);
    if (pos != s_geometry.end())
        return *pos->second;
    auto geometry = make_shared<GembloQGeometry>(nu_players);
    s_geometry.insert({nu_players, geometry});
    return *geometry;
}

auto GembloQGeometry::get_adj_coord(int x, int y) const -> AdjCoordList
{
    AdjCoordList l;
    l.push_back({x + 1, y});
    l.push_back({x - 1, y});
    switch (get_point_type(x, y))
    {
    case 0:
    case 3:
        l.push_back({x, y - 1});
        break;
    case 1:
    case 2:
        l.push_back({x, y + 1});
        break;
    }
    return l;
}

auto GembloQGeometry::get_diag_coord(int x, int y) const -> DiagCoordList
{
    // See Geometry::get_diag_coord() about advantageous ordering of the list
    DiagCoordList l;
    switch (get_point_type(x, y))
    {
    case 0:
        l.push_back({x + 2, y - 1});
        l.push_back({x - 1, y + 1});
        l.push_back({x - 1, y - 1});
        l.push_back({x, y + 1});
        l.push_back({x + 3, y});
        l.push_back({x - 2, y + 1});
        l.push_back({x + 1, y + 1});
        l.push_back({x + 3, y - 1});
        l.push_back({x - 2, y});
        l.push_back({x + 2, y});
        l.push_back({x + 1, y - 1});
        break;
    case 1:
        l.push_back({x - 2, y + 1});
        l.push_back({x + 1, y - 1});
        l.push_back({x + 1, y + 1});
        l.push_back({x, y - 1});
        l.push_back({x - 3, y});
        l.push_back({x + 2, y - 1});
        l.push_back({x - 1, y - 1});
        l.push_back({x - 3, y + 1});
        l.push_back({x + 2, y});
        l.push_back({x - 2, y});
        l.push_back({x - 1, y + 1});
        break;
    case 2:
        l.push_back({x - 2, y - 1});
        l.push_back({x + 3, y + 1});
        l.push_back({x - 1, y + 1});
        l.push_back({x, y - 1});
        l.push_back({x + 3, y});
        l.push_back({x + 2, y + 1});
        l.push_back({x + 1, y - 1});
        l.push_back({x - 2, y});
        l.push_back({x + 2, y});
        l.push_back({x - 1, y - 1});
        l.push_back({x + 1, y + 1});
        break;
    case 3:
        l.push_back({x - 3, y - 1});
        l.push_back({x + 2, y + 1});
        l.push_back({x + 1, y - 1});
        l.push_back({x, y + 1});
        l.push_back({x - 3, y});
        l.push_back({x - 2, y - 1});
        l.push_back({x - 1, y + 1});
        l.push_back({x + 2, y});
        l.push_back({x - 2, y});
        l.push_back({x + 1, y + 1});
        l.push_back({x - 1, y - 1});
        break;
    }
    return l;
}

unsigned GembloQGeometry::get_period_x() const
{
    return 4;
}

unsigned GembloQGeometry::get_period_y() const
{
    return 2;
}

unsigned GembloQGeometry::get_point_type(int x, int y) const
{
    return mod(x + 2 * static_cast<int>(y % 2 != 0), 4);
}

bool GembloQGeometry::init_is_onboard(unsigned x, unsigned y) const
{
    auto width = get_width();
    auto height = get_height();
    unsigned dy = min(y, height - y - 1);
    unsigned min_x = (width - 4 * m_edge) / 2 - 1 > 2 * dy ?
                (width - 4 * m_edge) / 2 - 1 - 2 * dy : 0;
    unsigned max_x = width - min_x - 1;
    return x >= min_x && x <= max_x;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

