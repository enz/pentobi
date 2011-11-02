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

    static const unsigned int max_size = (Point::max_width + 1) / 4;

    /** Create or reuse an already created geometry with a given size.
        @param sz The edge size of the hexagon. */
    static const TrigonGeometry* get(unsigned int sz);

protected:
    void init_is_onboard(Point p, bool& is_onboard) const;

    void init_adj_diag(Point p, NullTermList<Point, 4>& adj,
                       NullTermList<Point, 4>& diag) const;

private:
    static unique_ptr<TrigonGeometry> s_geometry[max_size + 1];

    unsigned int m_sz;

    TrigonGeometry(unsigned int size);
};

template<class P>
unique_ptr<TrigonGeometry<P>> TrigonGeometry<P>::s_geometry[max_size + 1];

template<class P>
TrigonGeometry<P>::TrigonGeometry(unsigned int sz)
{
    LIBBOARDGAME_ASSERT(sz <= max_size);
    m_sz = sz;
    Geometry<P>::init(sz * 4 - 1, sz * 2);
}

template<class P>
const TrigonGeometry<P>* TrigonGeometry<P>::get(unsigned int sz)
{
    if (s_geometry[sz].get() == 0)
        s_geometry[sz].reset(new TrigonGeometry(sz));
    return s_geometry[sz].get();
}

template<class P>
void TrigonGeometry<P>::init_is_onboard(Point p, bool& is_onboard) const
{
    unsigned int dy = min(p.get_y(), Geometry<P>::get_height() - p.get_y() - 1);
    unsigned int min_x = m_sz - dy - 1;
    unsigned int max_x = Geometry<P>::get_width() - min_x - 1;
    is_onboard = (p.get_x() >= min_x && p.get_x() <= max_x);
}

template<class P>
void TrigonGeometry<P>::init_adj_diag(Point p, NullTermList<Point, 4>& adj,
                                      NullTermList<Point, 4>& diag) const
{
    bool is_up;
    if (p.get_x() % 2 == 0)
        is_up = (p.get_y() % 2 != 0);
    else
        is_up = (p.get_y() % 2 == 0);
    typename NullTermList<Point, 4>::Init init_adj(adj);
    if (is_up)
    {
        if (is_onboard(p.get_down()))
            init_adj.push_back(p.get_down());
        if (is_onboard(p.get_left()))
            init_adj.push_back(p.get_left());
        if (is_onboard(p.get_right()))
            init_adj.push_back(p.get_right());
    }
    else
    {
        if (is_onboard(p.get_up()))
            init_adj.push_back(p.get_up());
        if (is_onboard(p.get_left()))
            init_adj.push_back(p.get_left());
        if (is_onboard(p.get_right()))
            init_adj.push_back(p.get_right());
    }
    init_adj.finish();
    typename NullTermList<Point, 4>::Init init_diag(diag);
    if (is_up)
    {
        if (is_onboard(p.get_up()))
            init_diag.push_back(p.get_up());
        if (is_onboard(p.get_down_left()))
            init_diag.push_back(p.get_down_left());
        if (is_onboard(p.get_down_right()))
            init_diag.push_back(p.get_down_right());
    }
    else
    {
        if (is_onboard(p.get_down()))
            init_diag.push_back(p.get_down());
        if (is_onboard(p.get_up_left()))
            init_diag.push_back(p.get_up_left());
        if (is_onboard(p.get_up_right()))
            init_diag.push_back(p.get_up_right());
    }
    init_diag.finish();
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_TRIGON_GEOMETRY_H
