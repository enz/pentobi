//-----------------------------------------------------------------------------
/** @file libboardgame_base/Geometry.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_GEOMETRY_H
#define LIBBOARDGAME_BASE_GEOMETRY_H

#include "CoordPoint.h"
#include "Point.h"
#include "libboardgame_util/NullTermList.h"

namespace libboardgame_base {

using namespace std;
using libboardgame_util::NullTermList;

//-----------------------------------------------------------------------------

/** Geometry data of a board with a given size.
    This class is a base class that uses virtual functions in its constructor
    that allow to restrict the shape of the board to a subset of the rectangle
    and/or to define different definitions of adjacent and diagonal neighbors
    of a point for geometries that are not a regular rectangular grid.
    @tparam P An instantiation of libboardgame_base::Point (or compatible
    class) */
template<class P>
class Geometry
{
public:
    typedef P Point;

    class Iterator
    {
        friend class Geometry;

    public:
        Iterator(const Geometry& g);

        const Point& operator*() const;

        const Point* operator->() const;

        operator bool() const;

        void operator++();

    private:
        const Point* m_p;

        const Point* m_end;
    };

    virtual ~Geometry();

    /** Return the point type if the board has different types of points.
        For example, in the geometry used in Blokus Trigon, there are two
        point types (0=downside triangle, 1=upside trinagle); in a regular
        rectangle, there is only one point type. By convention, 0 is the
        type of the point at (0,0).
        @param x The x coordinate (may be negative and/or outside the board).
        @param y The y coordinate (may be negative and/or outside the board). */
    virtual unsigned int get_point_type(int x, int y) const = 0;

    /** Get repeat interval for point types along the x axis.
        If the board has different point types, the layout of the point types
        repeats in this x interval. If the board has only one point type,
        the function should return 1. */
    virtual unsigned int get_period_x() const = 0;

    /** Get repeat interval for point types along the y axis.
        @see get_period_x(). */
    virtual unsigned int get_period_y() const = 0;

    unsigned int get_point_type(CoordPoint p) const;

    unsigned int get_point_type(Point p) const;

    bool is_onboard(Point p) const;

    bool is_onboard(CoordPoint p) const;

    unsigned int get_width() const;

    unsigned int get_height() const;

    /** Get list of on-board adjacent points. */
    const NullTermList<Point, 4>& get_adj(Point p) const;

    /** Get list of on-board diagonal points.
        Currently supports up to nine diagonal points as used on boards
        for Blokus Trigon. */
    const NullTermList<Point, 9>& get_diag(Point p) const;

    /** Get list of on-board adjacent and diagonal points.
        The adjacent points are first, diagobal points later in the list.
        Currently supports up to twelve diagonal points as used on boards
        for Blokus Trigon. */
    const NullTermList<Point, 12>& get_adj_diag(Point p) const;

    /** Get closest distance to first line. */
    unsigned int get_dist_to_edge(Point p) const;

    /** Get second closest distance to first line. */
    unsigned int get_second_dist_to_edge(Point p) const;

protected:
    Geometry();

    /** Initialize.
        Subclasses must call this function in their constructors. */
    void init(unsigned int width, unsigned int height);

    /** Initialize on-board points.
        This function is used in init() and allows the subclass to restrict the
        on-board points to a subset of the on-board points of a rectangle to
        support different board shapes. */
    virtual void init_is_onboard(Point p, bool& is_onboard) const = 0;

    /** Initialize adjacent and diagonal neighbors of an on-board point.
        This function is used in init() and allows the subclass to define other
        neighborhood relationships than the one of a regular rectangular grid
        (e.g. triangles or hexagonal fields). This function is used after
        the on-board status of all points has been initialized. */
    virtual void init_adj_diag(Point p, NullTermList<Point, 4>& adj,
                               NullTermList<Point, 9>& diag) const = 0;

private:
    unsigned int m_width;

    unsigned int m_height;

    bool m_is_onboard[Point::range];

    unsigned int m_dist_to_edge[Point::range];

    unsigned int m_second_dist_to_edge[Point::range];

    const Point* m_all_points_begin;

    const Point* m_all_points_end;

    unique_ptr<Point[]> m_all_points;

    NullTermList<Point, 4> m_adj[Point::range];

    NullTermList<Point, 9> m_diag[Point::range];

    NullTermList<Point, 12> m_adj_diag[Point::range];
};

template<class P>
inline Geometry<P>::Iterator::Iterator(const Geometry& g)
    : m_p(g.m_all_points_begin),
      m_end(g.m_all_points_end)
{
}

template<class P>
inline const P& Geometry<P>::Iterator::operator*() const
{
    LIBBOARDGAME_ASSERT(operator bool());
    return *m_p;
}

template<class P>
inline const P* Geometry<P>::Iterator::operator->() const
{
    LIBBOARDGAME_ASSERT(operator bool());
    return m_p;
}

template<class P>
inline Geometry<P>::Iterator::operator bool() const
{
    return m_p != m_end;
}

template<class P>
inline void Geometry<P>::Iterator::operator++()
{
    LIBBOARDGAME_ASSERT(operator bool());
    ++m_p;
}

template<class P>
Geometry<P>::Geometry()
{
}

template<class P>
Geometry<P>::~Geometry()
{
}

template<class P>
inline const NullTermList<P, 4>& Geometry<P>::get_adj(Point p) const
{
    LIBBOARDGAME_ASSERT(is_onboard(p));
    return m_adj[p.to_int()];
}

template<class P>
inline const NullTermList<P, 12>& Geometry<P>::get_adj_diag(Point p) const
{
    LIBBOARDGAME_ASSERT(is_onboard(p));
    return m_adj_diag[p.to_int()];
}

template<class P>
inline const NullTermList<P, 9>& Geometry<P>::get_diag(Point p) const
{
    LIBBOARDGAME_ASSERT(is_onboard(p));
    return m_diag[p.to_int()];
}

template<class P>
inline unsigned int Geometry<P>::get_dist_to_edge(Point p) const
{
    LIBBOARDGAME_ASSERT(is_onboard(p));
    return m_dist_to_edge[p.to_int()];
}

template<class P>
inline unsigned int Geometry<P>::get_height() const
{
    return m_height;
}

template<class P>
inline unsigned int Geometry<P>::get_point_type(Point p) const
{
    return get_point_type(p.get_x(), p.get_y());
}

template<class P>
inline unsigned int Geometry<P>::get_point_type(CoordPoint p) const
{
    return get_point_type(p.x, p.y);
}

template<class P>
inline unsigned int Geometry<P>::get_second_dist_to_edge(Point p) const
{
    LIBBOARDGAME_ASSERT(is_onboard(p));
    return m_second_dist_to_edge[p.to_int()];
}

template<class P>
inline unsigned int Geometry<P>::get_width() const
{
    return m_width;
}

template<class P>
void Geometry<P>::init(unsigned int width, unsigned int height)
{
    m_width = width;
    m_height = height;
    m_all_points.reset(new Point[width * height]);
    LIBBOARDGAME_ASSERT(width >= 1 && width <= Point::max_width);
    LIBBOARDGAME_ASSERT(height >= 1 && height <= Point::max_height);
    fill(m_is_onboard, m_is_onboard + Point::range, false);
    m_all_points_begin = m_all_points.get();
    Point* all_points_end = m_all_points.get();
    for (unsigned int y = 0; y < height; ++y)
        for (unsigned int x = 0; x < width; ++x)
        {
            Point p(x, y);
            init_is_onboard(p, m_is_onboard[p.to_int()]);
            if (is_onboard(p))
                *(all_points_end++) = p;
        }
    m_all_points_end = all_points_end;
    for (Iterator i(*this); i; ++i)
    {
        unsigned int j = (*i).to_int();
        init_adj_diag(*i, m_adj[j], m_diag[j]);
        typename NullTermList<Point, 12>::Init adj_diag(m_adj_diag[j]);
        for (typename NullTermList<Point, 4>::Iterator k(m_adj[j]); k; ++k)
            adj_diag.push_back(*k);
        for (typename NullTermList<Point, 9>::Iterator k(m_diag[j]); k; ++k)
            adj_diag.push_back(*k);
        adj_diag.finish();
        unsigned int x = (*i).get_x();
        unsigned int y = (*i).get_y();
        unsigned int dist_to_edge_x = min(width - x - 1, x);
        unsigned int dist_to_edge_y = min(height - y - 1, y);
        m_dist_to_edge[j] = min(dist_to_edge_x, dist_to_edge_y);
        m_second_dist_to_edge[j] = max(dist_to_edge_x, dist_to_edge_y);
    }
}

template<class P>
inline bool Geometry<P>::is_onboard(Point p) const
{
    return m_is_onboard[p.to_int()];
}

template<class P>
bool Geometry<P>::is_onboard(CoordPoint p) const
{
    return p.is_onboard(m_width, m_height) && is_onboard(Point(p.x, p.y));
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_GEOMETRY_H
