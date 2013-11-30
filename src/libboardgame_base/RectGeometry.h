//-----------------------------------------------------------------------------
/** @file libboardgame_base/RectGeometry.h
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_RECT_GEOMETRY_H
#define LIBBOARDGAME_BASE_RECT_GEOMETRY_H

#include <memory>
#include "Geometry.h"
#include "libboardgame_util/Unused.h"

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Geometry of a regular rectangular grid.
    @tparam P An instantiation of libboardgame_base::Point */
template<class P>
class RectGeometry
    : public Geometry<P>
{
public:
    typedef P Point;

    /** Create or reuse an already created geometry with a given size. */
    static const RectGeometry* get(unsigned width, unsigned height);

    RectGeometry(unsigned width, unsigned height);

    unsigned get_point_type(int x, int y) const override;

    unsigned get_period_x() const override;

    unsigned get_period_y() const override;

protected:
    void init_is_onboard(Point p, bool& is_onboard) const;

    void init_adj_diag(Point p, NullTermList<Point, 4>& adj,
                       NullTermList<Point, 9>& diag) const;

private:
    static unique_ptr<RectGeometry>
                        s_geometry[Point::max_width + 1][Point::max_height + 1];
};

template<class P>
unique_ptr<RectGeometry<P>>
               RectGeometry<P>::s_geometry[P::max_width + 1][P::max_height + 1];

template<class P>
RectGeometry<P>::RectGeometry(unsigned width, unsigned height)
{
    Geometry<P>::init(width, height);
}

template<class P>
const RectGeometry<P>* RectGeometry<P>::get(unsigned width, unsigned height)
{
    if (! s_geometry[width][height])
        s_geometry[width][height].reset(new RectGeometry(width, height));
    return s_geometry[width][height].get();
}

template<class P>
unsigned RectGeometry<P>::get_period_x() const
{
    return 1;
}

template<class P>
unsigned RectGeometry<P>::get_period_y() const
{
    return 1;
}

template<class P>
unsigned RectGeometry<P>::get_point_type(int x, int y) const
{
    LIBBOARDGAME_UNUSED(x);
    LIBBOARDGAME_UNUSED(y);
    return 0;
}

template<class P>
void RectGeometry<P>::init_is_onboard(Point p, bool& is_onboard) const
{
    LIBBOARDGAME_UNUSED(p);
    is_onboard = true;
}

template<class P>
void RectGeometry<P>::init_adj_diag(Point p, NullTermList<Point, 4>& adj,
                                    NullTermList<Point, 9>& diag) const
{
    unsigned width = this->get_width();
    unsigned height = this->get_height();
    unsigned x = p.get_x();
    unsigned y = p.get_y();
    {
        typename NullTermList<Point, 4>::Init init_adj(adj);
        if (x > 0)
            init_adj.push_back(p.get_left());
        if (x < width - 1)
            init_adj.push_back(p.get_right());
        if (y > 0)
            init_adj.push_back(p.get_down());
        if (y < height - 1)
            init_adj.push_back(p.get_up());
        init_adj.finish();
    }
    {
        typename NullTermList<Point, 9>::Init init_diag(diag);
        if (x > 0 && y < height - 1)
            init_diag.push_back(p.get_up_left());
        if (x < width - 1 && y < height - 1)
            init_diag.push_back(p.get_up_right());
        if (x > 0 && y > 0)
            init_diag.push_back(p.get_down_left());
        if (x < width - 1 && y > 0)
            init_diag.push_back(p.get_down_right());
        init_diag.finish();
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_RECT_GEOMETRY_H
