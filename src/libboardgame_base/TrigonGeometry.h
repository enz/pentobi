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
    There are two point types: 0=downward triangle, 1=upward triangle.
    @tparam P An instantiation of libboardgame_base::Point */
template<class P>
class TrigonGeometry
    : public Geometry<P>
{
public:
    typedef P Point;

    /** Create or reuse an already created geometry with a given size.
        @param sz The edge size of the hexagon. */
    static const TrigonGeometry& get(unsigned sz);

    unsigned get_point_type(int x, int y) const override;

    unsigned get_period_x() const override;

    unsigned get_period_y() const override;

protected:
    void init_is_onboard(Point p, bool& is_onboard) const;

    void init_adj_diag(Point p, NullTermList<Point, 4>& adj,
                       NullTermList<Point, 9>& diag) const;

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
        if (x % 2 != 0)
            return (y % 2 == 0 ? 1 : 0);
        else
            return (y % 2 != 0 ? 1 : 0);
    }
    else
    {
        if (x % 2 == 0)
            return (y % 2 == 0 ? 1 : 0);
        else
            return (y % 2 != 0 ? 1 : 0);
    }
}

template<class P>
void TrigonGeometry<P>::init_is_onboard(Point p, bool& is_onboard) const
{
    auto width = this->get_width();
    auto height = this->get_height();
    auto x = p.get_x(width);
    auto y = p.get_y(width);
    unsigned dy = min(y, height - y - 1);
    unsigned min_x = m_sz - dy - 1;
    unsigned max_x = width - min_x - 1;
    is_onboard = (x >= min_x && x <= max_x);
}

template<class P>
void TrigonGeometry<P>::init_adj_diag(Point p, NullTermList<Point, 4>& adj,
                                      NullTermList<Point, 9>& diag) const
{
    auto width = this->get_width();
    auto height = this->get_height();
    auto x = p.get_x(width);
    auto y = p.get_y(width);
    auto type = Geometry<P>::get_point_type(p);
    {
        typename NullTermList<Point, 4>::Init init_adj(adj);
        if (type == 0)
        {
            if (y > 0 && this->is_onboard(p.get_up(width)))
                init_adj.push_back(p.get_up(width));
            if (x > 0 && this->is_onboard(p.get_left()))
                init_adj.push_back(p.get_left());
            if (x < width - 1 && this->is_onboard(p.get_right()))
                init_adj.push_back(p.get_right());
        }
        else
        {
            if (x > 0 && this->is_onboard(p.get_left()))
                init_adj.push_back(p.get_left());
            if (x < width - 1 && this->is_onboard(p.get_right()))
                init_adj.push_back(p.get_right());
            if (y < height - 1 && this->is_onboard(p.get_down(width)))
                init_adj.push_back(p.get_down(width));
        }
        init_adj.finish();
    }
    {
        typename NullTermList<Point, 9>::Init init_diag(diag);
        if (type == 0)
        {
            // The order does not matter logically but it is better to put
            // far away 2nd order adjacent points first because it slightly
            // increases the efficiency of libpentobi_base::BoardConst, which
            // uses the forbidden status of the first few points from this list
            // during move generation and those points can reject more moves.
            if (x > 1 && this->is_onboard(p.get_left().get_left()))
                init_diag.push_back(p.get_left().get_left());
            if (x < width - 2 && this->is_onboard(p.get_right().get_right()))
                init_diag.push_back(p.get_right().get_right());
            if (x > 0 && y < height - 1
                    && this->is_onboard(p.get_down_left(width)))
                init_diag.push_back(p.get_down_left(width));
            if (x < width - 1 && y < height - 1
                    && this->is_onboard(p.get_down_right(width)))
                init_diag.push_back(p.get_down_right(width));
            if (x < width - 1 && y > 0
                    && this->is_onboard(p.get_up_right(width)))
                init_diag.push_back(p.get_up_right(width));
            if (x > 0 && y > 0
                    && this->is_onboard(p.get_up_left(width)))
                init_diag.push_back(p.get_up_left(width));
            if (y < height - 1 && this->is_onboard(p.get_down(width)))
                init_diag.push_back(p.get_down(width));
            if (x > 1 && y > 0
                    && this->is_onboard(p.get_up_left(width).get_left()))
                init_diag.push_back(p.get_up_left(width).get_left());
            if (x < width - 2 && y > 0
                    && this->is_onboard(p.get_up_right(width).get_right()))
                init_diag.push_back(p.get_up_right(width).get_right());
        }
        else
        {
            // See comment at type == 0 for the order of moves.
            if (x > 1 && this->is_onboard(p.get_left().get_left()))
                init_diag.push_back(p.get_left().get_left());
            if (x < width - 2 && this->is_onboard(p.get_right().get_right()))
                init_diag.push_back(p.get_right().get_right());
            if (x > 0 && y > 0
                    && this->is_onboard(p.get_up_left(width)))
                init_diag.push_back(p.get_up_left(width));
            if (x < width - 1 && y > 0
                    && this->is_onboard(p.get_up_right(width)))
                init_diag.push_back(p.get_up_right(width));
            if (x < width - 1 && y < height - 1
                    && this->is_onboard(p.get_down_right(width)))
                init_diag.push_back(p.get_down_right(width));
            if (x > 0 && y < height - 1
                    && this->is_onboard(p.get_down_left(width)))
                init_diag.push_back(p.get_down_left(width));
            if (y > 0 && this->is_onboard(p.get_up(width)))
                init_diag.push_back(p.get_up(width));
            if (x > 1 && y < height - 1
                    && this->is_onboard(p.get_down_left(width).get_left()))
                init_diag.push_back(p.get_down_left(width).get_left());
            if (x < width - 2 && y < height - 1
                    && this->is_onboard(p.get_down_right(width).get_right()))
                init_diag.push_back(p.get_down_right(width).get_right());
        }
        init_diag.finish();
    }

}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_TRIGON_GEOMETRY_H
