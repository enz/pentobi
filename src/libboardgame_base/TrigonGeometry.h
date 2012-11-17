//-----------------------------------------------------------------------------
/** @file libboardgame_base/TrigonGeometry.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_TRIGON_GEOMETRY_H
#define LIBBOARDGAME_BASE_TRIGON_GEOMETRY_H

#include <memory>
#include "Geometry.h"

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Geometry as used in the game Blokus Trigon.
    The board is a hexagon consisting of triangles. The coordinates are like
    in this example of a hexagon with edge size 3:
    <tt>
    6     / \ / \ / \ / \
    5   / \ / \ / \ / \ / \
    4 / \ / \ / \ / \ / \ / \
    3 \ / \ / \ / \ / \ / \ /
    2   \ / \ / \ / \ / \ /
    1     \ / \ / \ / \ /
       A B C D E F G H I J K
    </tt>
    @tparam P An instantiation of libboardgame_base::Point */
template<class P>
class TrigonGeometry
    : public Geometry<P>
{
public:
    typedef P Point;

    static const unsigned max_size = (Point::max_width + 1) / 4;

    /** Create or reuse an already created geometry with a given size.
        @param sz The edge size of the hexagon. */
    static const TrigonGeometry* get(unsigned sz);

    unsigned get_point_type(int x, int y) const;

    unsigned get_period_x() const;

    unsigned get_period_y() const;

protected:
    void init_is_onboard(Point p, bool& is_onboard) const;

    void init_adj_diag(Point p, NullTermList<Point, 4>& adj,
                       NullTermList<Point, 9>& diag) const;

private:
    static unique_ptr<TrigonGeometry> s_geometry[max_size + 1];

    unsigned m_sz;

    TrigonGeometry(unsigned size);
};

template<class P>
unique_ptr<TrigonGeometry<P>> TrigonGeometry<P>::s_geometry[max_size + 1];

template<class P>
TrigonGeometry<P>::TrigonGeometry(unsigned sz)
{
    LIBBOARDGAME_ASSERT(sz <= max_size);
    m_sz = sz;
    Geometry<P>::init(sz * 4 - 1, sz * 2);
}

template<class P>
const TrigonGeometry<P>* TrigonGeometry<P>::get(unsigned sz)
{
    if (s_geometry[sz].get() == 0)
        s_geometry[sz].reset(new TrigonGeometry(sz));
    return s_geometry[sz].get();
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
            return (y % 2 == 0 ? 0 : 1);
        else
            return (y % 2 != 0 ? 0 : 1);
    }
    else
    {
        if (x % 2 == 0)
            return (y % 2 == 0 ? 0 : 1);
        else
            return (y % 2 != 0 ? 0 : 1);
    }
}

template<class P>
void TrigonGeometry<P>::init_is_onboard(Point p, bool& is_onboard) const
{
    unsigned dy = min(p.get_y(), Geometry<P>::get_height() - p.get_y() - 1);
    unsigned min_x = m_sz - dy - 1;
    unsigned max_x = Geometry<P>::get_width() - min_x - 1;
    is_onboard = (p.get_x() >= min_x && p.get_x() <= max_x);
}

template<class P>
void TrigonGeometry<P>::init_adj_diag(Point p, NullTermList<Point, 4>& adj,
                                      NullTermList<Point, 9>& diag) const
{
    unsigned width = this->get_width();
    unsigned height = this->get_height();
    unsigned x = p.get_x();
    unsigned y = p.get_y();
    unsigned type = Geometry<P>::get_point_type(p);
    {
        typename NullTermList<Point, 4>::Init init_adj(adj);
        if (type == 0)
        {
            if (y < height - 1 && this->is_onboard(p.get_up()))
                init_adj.push_back(p.get_up());
            if (x > 0 && this->is_onboard(p.get_left()))
                init_adj.push_back(p.get_left());
            if (x < width - 1 && this->is_onboard(p.get_right()))
                init_adj.push_back(p.get_right());
        }
        else
        {
            if (y > 0 && this->is_onboard(p.get_down()))
                init_adj.push_back(p.get_down());
            if (x > 0 && this->is_onboard(p.get_left()))
                init_adj.push_back(p.get_left());
            if (x < width - 1 && this->is_onboard(p.get_right()))
                init_adj.push_back(p.get_right());
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
            if (x > 0 && y > 0 && this->is_onboard(p.get_down_left()))
                init_diag.push_back(p.get_down_left());
            if (x < width - 1 && y > 0 && this->is_onboard(p.get_down_right()))
                init_diag.push_back(p.get_down_right());
            if (x < width - 1 && y < height - 1
                && this->is_onboard(p.get_up_right()))
                init_diag.push_back(p.get_up_right());
            if (x > 0 && y < height - 1 && this->is_onboard(p.get_up_left()))
                init_diag.push_back(p.get_up_left());
            if (y > 0 && this->is_onboard(p.get_down()))
                init_diag.push_back(p.get_down());
            if (x > 1 && y < height - 1
                && this->is_onboard(p.get_up_left().get_left()))
                init_diag.push_back(p.get_up_left().get_left());
            if (x < width - 2 && y < height - 1
                && this->is_onboard(p.get_up_right().get_right()))
                init_diag.push_back(p.get_up_right().get_right());
        }
        else
        {
            // See comment at type == 0 for the order of moves.
            if (x > 1 && this->is_onboard(p.get_left().get_left()))
                init_diag.push_back(p.get_left().get_left());
            if (x < width - 2 && this->is_onboard(p.get_right().get_right()))
                init_diag.push_back(p.get_right().get_right());
            if (x > 0 && y < height - 1 && this->is_onboard(p.get_up_left()))
                init_diag.push_back(p.get_up_left());
            if (x < width - 1 && y < height - 1
                && this->is_onboard(p.get_up_right()))
                init_diag.push_back(p.get_up_right());
            if (x < width - 1 && y > 0 && this->is_onboard(p.get_down_right()))
                init_diag.push_back(p.get_down_right());
            if (x > 0 && y > 0 && this->is_onboard(p.get_down_left()))
                init_diag.push_back(p.get_down_left());
            if (y < height - 1 && this->is_onboard(p.get_up()))
                init_diag.push_back(p.get_up());
            if (x > 1 && y > 0
                && this->is_onboard(p.get_down_left().get_left()))
                init_diag.push_back(p.get_down_left().get_left());
            if (x < width - 2 && y > 0
                && this->is_onboard(p.get_down_right().get_right()))
                init_diag.push_back(p.get_down_right().get_right());
        }
        init_diag.finish();
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_TRIGON_GEOMETRY_H
