//-----------------------------------------------------------------------------
/** @file libboardgame_base/Geometry.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_GEOMETRY_H
#define LIBBOARDGAME_BASE_GEOMETRY_H

#include <memory>
#include <boost/scoped_array.hpp>
#include "Point.h"
#include "libboardgame_util/NullTermList.h"

namespace libboardgame_base {

using namespace std;
using boost::scoped_array;
using libboardgame_util::NullTermList;

//-----------------------------------------------------------------------------

/** Geometry data of a board with a given size.
    @tparam P An instantiation of libboardgame_base::Point */
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

    static const Geometry* get(unsigned int width, unsigned int height);

    bool is_onboard(Point p) const;

    unsigned int get_width() const;

    unsigned int get_height() const;

    /** Get list of on-board adjacent points (up to four) */
    const NullTermList<Point, 4>& get_adj(Point p) const;

    /** Get list of on-board diagonal points (up to four) */
    const NullTermList<Point, 4>& get_diag(Point p) const;

    /** Get list of on-board adjacent and diagonal points (up to eight) */
    const NullTermList<Point, 8>& get_adj_diag(Point p) const;

    /** Get closest distance to first line. */
    unsigned int get_dist_to_edge(Point p) const;

    /** Get second closest distance to first line. */
    unsigned int get_second_dist_to_edge(Point p) const;

private:
    static auto_ptr<Geometry>
                        s_geometry[Point::max_width + 1][Point::max_height + 1];

    unsigned int m_width;

    unsigned int m_height;

    bool m_is_onboard[Point::range];

    unsigned int m_dist_to_edge[Point::range];

    unsigned int m_second_dist_to_edge[Point::range];

    const Point* m_all_points_begin;

    const Point* m_all_points_end;

    scoped_array<Point> m_all_points;

    NullTermList<Point, 4> m_adj[Point::range];

    NullTermList<Point, 4> m_diag[Point::range];

    NullTermList<Point, 8> m_adj_diag[Point::range];

    Geometry(unsigned int width, unsigned int height);
};

template<class P>
auto_ptr<Geometry<P>>
                   Geometry<P>::s_geometry[P::max_width + 1][P::max_height + 1];

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
Geometry<P>::Geometry(unsigned int width, unsigned int height)
    : m_width(width),
      m_height(height),
      m_all_points(new Point[width * height])
{
    LIBBOARDGAME_ASSERT(width >= 1 && width <= Point::max_width);
    LIBBOARDGAME_ASSERT(height >= 1 && height <= Point::max_height);
    fill(m_is_onboard, m_is_onboard + Point::range, false);
    Point* all_points_end = m_all_points.get();
    for (unsigned int y = 0; y < height; ++y)
        for (unsigned int x = 0; x < width; ++x)
        {
            Point p(x, y);
            *(all_points_end++) = p;
            m_is_onboard[p.to_int()] = true;
        }
    for (unsigned int y = 0; y < height; ++y)
        for (unsigned int x = 0; x < width; ++x)
        {
            Point p(x, y);
            unsigned int i = p.to_int();
            {
                typename NullTermList<Point, 4>::Init adj(m_adj[i]);
                LIBBOARDGAME_FOREACH_ADJ(p, p_adj,
                    if (is_onboard(p_adj))
                        adj.push_back(p_adj););
                adj.finish();
            }
            {
                typename NullTermList<Point, 4>::Init diag(m_diag[i]);
                LIBBOARDGAME_FOREACH_DIAG(p, p_diag,
                    if (is_onboard(p_diag))
                        diag.push_back(p_diag););
                diag.finish();
            }
            {
                typename NullTermList<Point, 8>::Init adj_diag(m_adj_diag[i]);
                LIBBOARDGAME_FOREACH_ADJ_DIAG(p, p_adj_diag,
                    if (is_onboard(p_adj_diag))
                        adj_diag.push_back(p_adj_diag););
                adj_diag.finish();
            }
            unsigned int dist_to_edge_x = min(width - x - 1, x);
            unsigned int dist_to_edge_y = min(height - y - 1, y);
            m_dist_to_edge[i] = min(dist_to_edge_x, dist_to_edge_y);
            m_second_dist_to_edge[i] = max(dist_to_edge_x, dist_to_edge_y);
        }
    m_all_points_begin = m_all_points.get();
    m_all_points_end = all_points_end;
}

template<class P>
const Geometry<P>* Geometry<P>::get(unsigned int width, unsigned int height)
{
    if (s_geometry[width][height].get() == 0)
        s_geometry[width][height].reset(new Geometry(width, height));
    return s_geometry[width][height].get();
}

template<class P>
inline const NullTermList<P, 4>& Geometry<P>::get_adj(Point p) const
{
    LIBBOARDGAME_ASSERT(is_onboard(p));
    return m_adj[p.to_int()];
}

template<class P>
inline const NullTermList<P, 8>& Geometry<P>::get_adj_diag(Point p) const
{
    LIBBOARDGAME_ASSERT(is_onboard(p));
    return m_adj_diag[p.to_int()];
}

template<class P>
inline const NullTermList<P, 4>& Geometry<P>::get_diag(Point p) const
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
inline bool Geometry<P>::is_onboard(Point p) const
{
    return m_is_onboard[p.to_int()];
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_GEOMETRY_H
