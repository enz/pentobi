//-----------------------------------------------------------------------------
/** @file libboardgame_base/RectGeometry.h */
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
    static const RectGeometry* get(unsigned int width, unsigned int height);

    RectGeometry(unsigned int width, unsigned int height);

    unsigned int get_point_type(int x, int y) const;

protected:
    void init_is_onboard(Point p, bool& is_onboard) const;

    void init_adj_diag(Point p, NullTermList<Point, 4>& adj,
                       NullTermList<Point, 4>& diag) const;

private:
    static unique_ptr<RectGeometry>
                        s_geometry[Point::max_width + 1][Point::max_height + 1];
};

template<class P>
unique_ptr<RectGeometry<P>>
               RectGeometry<P>::s_geometry[P::max_width + 1][P::max_height + 1];

template<class P>
RectGeometry<P>::RectGeometry(unsigned int width, unsigned int height)
{
    Geometry<P>::init(width, height);
}

template<class P>
const RectGeometry<P>* RectGeometry<P>::get(unsigned int width,
                                            unsigned int height)
{
    if (s_geometry[width][height].get() == 0)
        s_geometry[width][height].reset(new RectGeometry(width, height));
    return s_geometry[width][height].get();
}

template<class P>
unsigned int RectGeometry<P>::get_point_type(int x, int y) const
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
                                    NullTermList<Point, 4>& diag) const
{
    typename NullTermList<Point, 4>::Init init_adj(adj);
    LIBBOARDGAME_FOREACH_ADJ(p, p_adj,
        if (is_onboard(p_adj))
            init_adj.push_back(p_adj););
    init_adj.finish();
    typename NullTermList<Point, 4>::Init init_diag(diag);
    LIBBOARDGAME_FOREACH_DIAG(p, p_diag,
        if (is_onboard(p_diag))
            init_diag.push_back(p_diag););
    init_diag.finish();
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_RECT_GEOMETRY_H
