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
    @tparam P An instantiation of boardgame::Point */
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

        Iterator(unsigned int sz);

        const Point& operator*() const;

        const Point* operator->() const;

        operator bool() const;

        void operator++();

    private:
        const Point* m_p;

        const Point* m_end;
    };

    static const Geometry* get(unsigned int sz);

    unsigned int get_size() const;

    bool is_handicap_line(unsigned int i) const;

    bool is_handicap_point(Point p) const;

    /** Get list of on-board adjacent points (up to four) */
    const NullTermList<Point, 4>& get_adj(Point p) const;

    /** Get list of on-board adjacent and diagonal points (up to eight) */
    const NullTermList<Point, 8>& get_adj_diag(Point p) const;

    /** Get closest distance to first line. */
    unsigned int get_dist_to_edge(Point p) const;

    /** Get second closest distance to first line. */
    unsigned int get_second_dist_to_edge(Point p) const;

private:
    static auto_ptr<Geometry> s_geometry[Point::max_size + 1];

    unsigned int m_sz;

    unsigned int m_handicap_line_1;

    unsigned int m_handicap_line_2;

    unsigned int m_handicap_line_3;

    unsigned int m_dist_to_edge[Point::range];

    unsigned int m_second_dist_to_edge[Point::range];

    const Point* m_all_points_begin;

    const Point* m_all_points_end;

    scoped_array<Point> m_all_points;

    NullTermList<Point, 4> m_adj[Point::range];

    NullTermList<Point, 8> m_adj_diag[Point::range];

    Geometry(unsigned int sz);
};

template<class P>
auto_ptr<Geometry<P>> Geometry<P>::s_geometry[P::max_size + 1];

template<class P>
inline Geometry<P>::Iterator::Iterator(const Geometry& g)
    : m_p(g.m_all_points_begin),
      m_end(g.m_all_points_end)
{
}

template<class P>
inline Geometry<P>::Iterator::Iterator(unsigned int sz)
{
    const Geometry* g = Geometry<P>::get(sz);
    m_p = g->m_all_points_begin;
    m_end = g->m_all_points_end;
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
Geometry<P>::Geometry(unsigned int sz)
    : m_sz(sz),
      m_all_points(new Point[sz * sz])
{
    LIBBOARDGAME_ASSERT(sz >= 1 && sz <= Point::max_size);
    Point* all_points_end = m_all_points.get();
    for (unsigned int y = 0; y < sz; ++y)
        for (unsigned int x = 0; x < sz; ++x)
        {
            Point p(x, y);
            *(all_points_end++) = p;
            unsigned int i = p.to_int();
            {
                typename NullTermList<Point, 4>::Init adj(m_adj[i]);
                LIBBOARDGAME_FOREACH_ADJ(p, p_adj,
                    if (p_adj.is_onboard(sz))
                        adj.push_back(p_adj););
                adj.finish();
            }
            {
                typename NullTermList<Point, 8>::Init adj_diag(m_adj_diag[i]);
                LIBBOARDGAME_FOREACH_ADJ_DIAG(p, p_adj_diag,
                    if (p_adj_diag.is_onboard(sz))
                        adj_diag.push_back(p_adj_diag););
                adj_diag.finish();
            }
            unsigned int dist_to_edge_x = min(sz - x - 1, x);
            unsigned int dist_to_edge_y = min(sz - y - 1, y);
            m_dist_to_edge[i] = min(dist_to_edge_x, dist_to_edge_y);
            m_second_dist_to_edge[i] = max(dist_to_edge_x, dist_to_edge_y);
        }
    m_all_points_begin = m_all_points.get();
    m_all_points_end = all_points_end;
    if (sz >= 13)
    {
        m_handicap_line_1 = 3;
        m_handicap_line_3 = sz - 4;
    }
    else if (sz >= 7)
    {
        m_handicap_line_1 = 2;
        m_handicap_line_3 = sz - 3;
    }
    else
    {
        m_handicap_line_1 = -1;
        m_handicap_line_3 = -1;
    }
    if (sz >= 9 && sz % 2 != 0)
        m_handicap_line_2 = sz / 2;
    else
        m_handicap_line_2 = -1;
}

template<class P>
const Geometry<P>* Geometry<P>::get(unsigned int sz)
{
    if (s_geometry[sz].get() == 0)
        s_geometry[sz].reset(new Geometry(sz));
    return s_geometry[sz].get();
}

template<class P>
inline const NullTermList<P, 4>& Geometry<P>::get_adj(Point p) const
{
    LIBBOARDGAME_ASSERT(p.is_onboard(m_sz));
    return m_adj[p.to_int()];
}

template<class P>
inline const NullTermList<P, 8>& Geometry<P>::get_adj_diag(Point p) const
{
    LIBBOARDGAME_ASSERT(p.is_onboard(m_sz));
    return m_adj_diag[p.to_int()];
}

template<class P>
inline unsigned int Geometry<P>::get_dist_to_edge(Point p) const
{
    LIBBOARDGAME_ASSERT(p.is_onboard(m_sz));
    return m_dist_to_edge[p.to_int()];
}

template<class P>
inline unsigned int Geometry<P>::get_second_dist_to_edge(Point p) const
{
    LIBBOARDGAME_ASSERT(p.is_onboard(m_sz));
    return m_second_dist_to_edge[p.to_int()];
}

template<class P>
inline unsigned int Geometry<P>::get_size() const
{
    return m_sz;
}

template<class P>
inline bool Geometry<P>::is_handicap_line(unsigned int i) const
{
    LIBBOARDGAME_ASSERT(i < Point::max_size);
    return (i == m_handicap_line_1 || i == m_handicap_line_2
            || i == m_handicap_line_3);
}

template<class P>
inline bool Geometry<P>::is_handicap_point(Point p) const
{
    return is_handicap_line(p.get_x()) && is_handicap_line(p.get_y());
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_GEOMETRY_H
