//-----------------------------------------------------------------------------
/** @file libpentobi_base/TrigonGeometry.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TrigonGeometry.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

map<unsigned, shared_ptr<TrigonGeometry>> TrigonGeometry::s_geometry;

TrigonGeometry::TrigonGeometry(unsigned sz)
{
    m_sz = sz;
    Geometry::init(sz * 4 - 1, sz * 2);
}

const TrigonGeometry& TrigonGeometry::get(unsigned sz)
{
    auto pos = s_geometry.find(sz);
    if (pos != s_geometry.end())
        return *pos->second;
    shared_ptr<TrigonGeometry> geometry(new TrigonGeometry(sz));
    return *s_geometry.insert(make_pair(sz, geometry)).first->second;
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
        else
            return y % 2 != 0 ? 1 : 0;
    }
    else
    {
        if (x % 2 != 0)
            return y % 2 == 0 ? 1 : 0;
        else
            return y % 2 != 0 ? 1 : 0;
    }
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

void TrigonGeometry::init_adj_diag(Point p, AdjList& adj, DiagList& diag) const
{
    auto width = get_width();
    auto height = get_height();
    auto x = get_x(p);
    auto y = get_y(p);
    auto type = Geometry::get_point_type(p);
    if (type == 0)
    {
        if (x > 0 && this->is_onboard(x - 1, y))
            adj.push_back(this->get_point(x - 1, y));
        if (x < width - 1 && this->is_onboard(x + 1, y))
            adj.push_back(this->get_point(x + 1, y));
        if (y < height - 1 && this->is_onboard(x, y + 1))
            adj.push_back(this->get_point(x, y + 1));

        // The order does not matter logically but it is better to put far away
        // 2nd order adjacent points first because it slightly increases the
        // efficiency of libpentobi_base::BoardConst, which uses the forbidden
        // status of the first few points from this list during move generation
        // and those points can reject more moves.
        if (x > 1 && this->is_onboard(x - 2, y))
            diag.push_back(this->get_point(x - 2, y));
        if (x < width - 2 && this->is_onboard(x + 2, y))
            diag.push_back(this->get_point(x + 2, y));
        if (x > 0 && y > 0
                && this->is_onboard(x - 1, y - 1))
            diag.push_back(this->get_point(x - 1, y - 1));
        if (x < width - 1 && y > 0 && this->is_onboard(x + 1, y - 1))
            diag.push_back(this->get_point(x + 1, y - 1));
        if (x < width - 1 && y < height - 1
                && this->is_onboard(x + 1, y + 1))
            diag.push_back(this->get_point(x + 1, y + 1));
        if (x > 0 && y < height - 1 && this->is_onboard(x - 1, y + 1))
            diag.push_back(this->get_point(x - 1, y + 1));
        if (y > 0 && this->is_onboard(x, y - 1))
            diag.push_back(this->get_point(x, y - 1));
        if (x > 1 && y < height - 1 && this->is_onboard(x - 2, y + 1))
            diag.push_back(this->get_point(x - 2, y + 1));
        if (x < width - 2 && y < height - 1
                && this->is_onboard(x + 2, y + 1))
            diag.push_back(this->get_point(x + 2, y + 1));
    }
    else
    {
        if (y > 0 && this->is_onboard(x, y - 1))
            adj.push_back(this->get_point(x, y - 1));
        if (x > 0 && this->is_onboard(x - 1, y))
            adj.push_back(this->get_point(x - 1, y));
        if (x < width - 1 && this->is_onboard(x + 1, y))
            adj.push_back(this->get_point(x + 1, y));

        // See comment at type == 0 for the order of moves.
        if (x > 1 && this->is_onboard(x - 2, y))
            diag.push_back(this->get_point(x - 2, y));
        if (x < width - 2 && this->is_onboard(x + 2, y))
            diag.push_back(this->get_point(x + 2, y));
        if (x > 0 && y < height - 1 && this->is_onboard(x - 1, y + 1))
            diag.push_back(this->get_point(x - 1, y + 1));
        if (x < width - 1 && y < height - 1
                && this->is_onboard(x + 1, y + 1))
            diag.push_back(this->get_point(x + 1, y + 1));
        if (x < width - 1 && y > 0 && this->is_onboard(x + 1, y - 1))
            diag.push_back(this->get_point(x + 1, y - 1));
        if (x > 0 && y > 0 && this->is_onboard(x - 1, y - 1))
            diag.push_back(this->get_point(x - 1, y - 1));
        if (y < height - 1 && this->is_onboard(x, y + 1))
            diag.push_back(this->get_point(x, y + 1));
        if (x > 1 && y > 0 && this->is_onboard(x - 2, y - 1))
            diag.push_back(this->get_point(x - 2, y - 1));
        if (x < width - 2 && y > 0 && this->is_onboard(x + 2, y - 1))
            diag.push_back(this->get_point(x + 2, y - 1));
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

