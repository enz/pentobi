//-----------------------------------------------------------------------------
/** @file libboardgame_base/TrigonGeometry.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_TRIGON_GEOMETRY_H
#define LIBBOARDGAME_BASE_TRIGON_GEOMETRY_H

#include <algorithm>
#include <map>
#include <memory>
#include "Geometry.h"

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Geometry as used in the game Blokus Trigon.
    The board is a hexagon consisting of triangles. The coordinates are like
    in this example of a hexagon with edge size 3:
    <tt>
       0 1 2 3 4 5 6 7 8 9 10
    0     / \ / \ / \ / \
    1   / \ / \ / \ / \ / \
    2 / \ / \ / \ / \ / \ / \
    3 \ / \ / \ / \ / \ / \ /
    4   \ / \ / \ / \ / \ /
    5     \ / \ / \ / \ /
    </tt>
    There are two point types: 0=upward triangle, 1=downward triangle.
    @tparam P An instantiation of libboardgame_base::Point */
template<class P>
class TrigonGeometry
    : public Geometry<P>
{
public:
    typedef P Point;

    using AdjList = typename Geometry<P>::AdjList;

    using DiagList = typename Geometry<P>::DiagList;

    /** Create or reuse an already created geometry with a given size.
        @param sz The edge size of the hexagon. */
    static const TrigonGeometry& get(unsigned sz);

    unsigned get_point_type(int x, int y) const override;

    unsigned get_period_x() const override;

    unsigned get_period_y() const override;

protected:
    bool init_is_onboard(unsigned x, unsigned y) const override;

    void init_adj_diag(Point p, AdjList& adj, DiagList& diag) const override;

private:
    /** Stores already created geometries by size. */
    static map<unsigned, shared_ptr<TrigonGeometry>> s_geometry;

    unsigned m_sz;

    TrigonGeometry(unsigned size);
};

template<class P>
map<unsigned, shared_ptr<TrigonGeometry<P>>> TrigonGeometry<P>::s_geometry;

template<class P>
TrigonGeometry<P>::TrigonGeometry(unsigned sz)
{
    m_sz = sz;
    Geometry<P>::init(sz * 4 - 1, sz * 2);
}

template<class P>
const TrigonGeometry<P>& TrigonGeometry<P>::get(unsigned sz)
{
    auto pos = s_geometry.find(sz);
    if (pos != s_geometry.end())
        return *pos->second;
    shared_ptr<TrigonGeometry> geometry(new TrigonGeometry(sz));
    return *s_geometry.insert(make_pair(sz, geometry)).first->second;
}

template<class P>
unsigned TrigonGeometry<P>::get_period_x() const
{
    return 2;
}

template<class P>
unsigned TrigonGeometry<P>::get_period_y() const
{
    return 2;
}

template<class P>
unsigned TrigonGeometry<P>::get_point_type(int x, int y) const
{
    if (m_sz % 2 == 0)
    {
        if (x % 2 == 0)
            return (y % 2 == 0 ? 1 : 0);
        else
            return (y % 2 != 0 ? 1 : 0);
    }
    else
    {
        if (x % 2 != 0)
            return (y % 2 == 0 ? 1 : 0);
        else
            return (y % 2 != 0 ? 1 : 0);
    }
}

template<class P>
bool TrigonGeometry<P>::init_is_onboard(unsigned x, unsigned y) const
{
    auto width = this->get_width();
    auto height = this->get_height();
    unsigned dy = min(y, height - y - 1);
    unsigned min_x = m_sz - dy - 1;
    unsigned max_x = width - min_x - 1;
    return x >= min_x && x <= max_x;
}

template<class P>
void TrigonGeometry<P>::init_adj_diag(Point p, AdjList& adj,
                                      DiagList& diag) const
{
    auto width = this->get_width();
    auto height = this->get_height();
    auto x = this->get_x(p);
    auto y = this->get_y(p);
    auto type = Geometry<P>::get_point_type(p);
    {
        if (type == 0)
        {
            if (x > 0 && this->is_onboard(x - 1, y))
                adj.push_back(this->get_point(x - 1, y));
            if (x < width - 1 && this->is_onboard(x + 1, y))
                adj.push_back(this->get_point(x + 1, y));
            if (y < height - 1 && this->is_onboard(x, y + 1))
                adj.push_back(this->get_point(x, y + 1));
        }
        else
        {
            if (y > 0 && this->is_onboard(x, y - 1))
                adj.push_back(this->get_point(x, y - 1));
            if (x > 0 && this->is_onboard(x - 1, y))
                adj.push_back(this->get_point(x - 1, y));
            if (x < width - 1 && this->is_onboard(x + 1, y))
                adj.push_back(this->get_point(x + 1, y));
        }
    }
    {
        if (type == 0)
        {
            // The order does not matter logically but it is better to put
            // far away 2nd order adjacent points first because it slightly
            // increases the efficiency of libpentobi_base::BoardConst, which
            // uses the forbidden status of the first few points from this list
            // during move generation and those points can reject more moves.
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
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_TRIGON_GEOMETRY_H
