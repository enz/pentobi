//-----------------------------------------------------------------------------
/** @file libboardgame_base/Geometry.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_GEOMETRY_H
#define LIBBOARDGAME_BASE_GEOMETRY_H

#include <sstream>
#include "CoordPoint.h"
#include "libboardgame_util/ArrayList.h"

namespace libboardgame_base {

using namespace std;
using libboardgame_util::ArrayList;

//-----------------------------------------------------------------------------

/** %Geometry data of a board with a given size.
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

    typedef typename Point::IntType IntType;

    /** On-board adjacent neighbors of a point. */
    typedef ArrayList<Point, 4, unsigned short> AdjList;

    /** On-board diagonal neighbors of a point
        Currently supports up to nine diagonal points as used on boards
        for Blokus Trigon. */
    typedef ArrayList<Point, 9, unsigned short> DiagList;

    /** Adjacent neighbors of a coordinate. */
    typedef ArrayList<CoordPoint, 4> AdjCoordList;

    /** Diagonal neighbors of a coordinate. */
    typedef ArrayList<CoordPoint, 9> DiagCoordList;

    class Iterator
    {
    public:
        Iterator(IntType i)
        {
            m_i = i;
        }

        bool operator==(Iterator it) const
        {
            return m_i == it.m_i;
        }

        bool operator!=(Iterator it) const
        {
            return m_i != it.m_i;
        }

        void operator++()
        {
            ++m_i;
        }

        Point operator*() const
        {
            return Point(m_i);
        }

    private:
        IntType m_i;
    };

    virtual ~Geometry();

    virtual AdjCoordList get_adj_coord(int x, int y) const = 0;

    virtual DiagCoordList get_diag_coord(int x, int y) const = 0;

    /** Return the point type if the board has different types of points.
        For example, in the geometry used in Blokus Trigon, there are two
        point types (0=upward triangle, 1=downward triangle); in a regular
        rectangle, there is only one point type. By convention, 0 is the
        type of the point at (0,0).
        @param x The x coordinate (may be negative and/or outside the board).
        @param y The y coordinate (may be negative and/or outside the board). */
    virtual unsigned get_point_type(int x, int y) const = 0;

    /** Get repeat interval for point types along the x axis.
        If the board has different point types, the layout of the point types
        repeats in this x interval. If the board has only one point type,
        the function should return 1. */
    virtual unsigned get_period_x() const = 0;

    /** Get repeat interval for point types along the y axis.
        @see get_period_x(). */
    virtual unsigned get_period_y() const = 0;

    Iterator begin() const { return Iterator(Point::begin_onboard); }

    Iterator end() const { return Iterator(get_range()); }

    unsigned get_point_type(CoordPoint p) const;

    unsigned get_point_type(Point p) const;

    bool is_onboard(unsigned x, unsigned y) const;

    bool is_onboard(CoordPoint p) const;

    /** Return the point at a given coordinate.
        @pre x < get_width()
        @pre y < get_height()
        @return The point or Point::null() if this coordinates are
        off-board. */
    Point get_point(unsigned x, unsigned y) const;

    unsigned get_width() const;

    unsigned get_height() const;

    /** Get range used for onboard points. */
    IntType get_range() const;

    unsigned get_x(Point p) const;

    unsigned get_y(Point p) const;

    bool from_string(const string& s, Point& p) const;

    const string& to_string(Point p) const;

    const AdjList& get_adj(Point p) const;

    const DiagList& get_diag(Point p) const;

    /** Iterate over adjacent points.
        Slightly faster than for(Point : get_adj()) because it knows that the
        list is not empty. */
    template<class FUNCTION>
    void for_each_adj(Point p, FUNCTION f) const;

    /** Iterate over diagonal points.
        Slightly faster than for(Point : get_diag()) because it knows that the
        list is not empty. */
    template<class FUNCTION>
    void for_each_diag(Point p, FUNCTION f) const;

protected:
    Geometry();

    /** Initialize.
        Subclasses must call this function in their constructors. */
    void init(unsigned width, unsigned height);

    /** Initialize on-board points.
        This function is used in init() and allows the subclass to restrict the
        on-board points to a subset of the on-board points of a rectangle to
        support different board shapes. It will only be called with x and
        y within the width and height of the geometry. */
    virtual bool init_is_onboard(unsigned x, unsigned y) const = 0;

private:
    AdjList m_adj[Point::range_onboard];

    DiagList m_diag[Point::range_onboard];

    IntType m_range;

    Point m_points[Point::max_width][Point::max_height];

    unsigned m_width;

    unsigned m_height;

    unsigned m_x[Point::range_onboard];

    unsigned m_y[Point::range_onboard];

    string m_string[Point::range];

#if LIBBOARDGAME_DEBUG
    bool is_valid(Point p) const;
#endif
};


template<class P>
Geometry<P>::Geometry()
{
}

template<class P>
Geometry<P>::~Geometry()
{
}

template<class P>
template<class FUNCTION>
inline void Geometry<P>::for_each_adj(Point p, FUNCTION f) const
{
    auto& l = get_adj(p);
    auto i = l.begin();
    auto end = l.end();
    LIBBOARDGAME_ASSERT(i != end);
    do
        f(*i);
    while (++i != end);
}

template<class P>
template<class FUNCTION>
inline void Geometry<P>::for_each_diag(Point p, FUNCTION f) const
{
    auto& l = get_diag(p);
    auto i = l.begin();
    auto end = l.end();
    LIBBOARDGAME_ASSERT(i != end);
    do
        f(*i);
    while (++i != end);
}

template<class P>
bool Geometry<P>::from_string(const string& s, Point& p) const
{
    istringstream in(s);
    unsigned x;
    unsigned y;
    if (Point::StringRep::read(in, m_width, m_height, x, y)
            && is_onboard(CoordPoint(x, y)))
    {
        p = get_point(x, y);
        return true;
    }
    return false;
}

template<class P>
inline auto Geometry<P>::get_adj(Point p) const -> const AdjList&
{
    LIBBOARDGAME_ASSERT(is_valid(p));
    return m_adj[p.to_int()];
}

template<class P>
inline auto Geometry<P>::get_diag(Point p) const -> const DiagList&
{
    LIBBOARDGAME_ASSERT(is_valid(p));
    return m_diag[p.to_int()];
}

template<class P>
inline unsigned Geometry<P>::get_height() const
{
    return m_height;
}

template<class P>
inline auto Geometry<P>::get_point(unsigned x, unsigned y) const -> Point
{
    LIBBOARDGAME_ASSERT(x < m_width);
    LIBBOARDGAME_ASSERT(y < m_height);
    return m_points[x][y];
}

template<class P>
inline unsigned Geometry<P>::get_point_type(Point p) const
{
    return get_point_type(get_x(p), get_y(p));
}

template<class P>
inline unsigned Geometry<P>::get_point_type(CoordPoint p) const
{
    return get_point_type(p.x, p.y);
}

template<class P>
inline auto Geometry<P>::get_range() const -> IntType
{
    return m_range;
}

template<class P>
inline unsigned Geometry<P>::get_width() const
{
    return m_width;
}

template<class P>
inline unsigned Geometry<P>::get_x(Point p) const
{
    LIBBOARDGAME_ASSERT(is_valid(p));
    return m_x[p.to_int()];
}

template<class P>
inline unsigned Geometry<P>::get_y(Point p) const
{
    LIBBOARDGAME_ASSERT(is_valid(p));
    return m_y[p.to_int()];
}

template<class P>
void Geometry<P>::init(unsigned width, unsigned height)
{
    LIBBOARDGAME_ASSERT(width >= 1);
    LIBBOARDGAME_ASSERT(height >= 1);
    LIBBOARDGAME_ASSERT(width <= Point::max_width);
    LIBBOARDGAME_ASSERT(height <= Point::max_height);
    m_width = width;
    m_height = height;
    m_string[Point::null().to_int()] = "null";
    IntType n = Point::begin_onboard;
    ostringstream ostr;
    for (unsigned y = 0; y < height; ++y)
        for (unsigned x = 0; x < width; ++x)
            if (init_is_onboard(x, y))
            {
                m_points[x][y] = Point(n);
                m_x[n] = x;
                m_y[n] = y;
                ostr.str("");
                Point::StringRep::write(ostr, x, y, width, height);
                m_string[n] = ostr.str();
                ++n;
            }
            else
                m_points[x][y] = Point::null();
    m_range = n;
    for (IntType i = Point::begin_onboard; i < m_range; ++i)
    {
        Point p(i);
        auto x = get_x(p);
        auto y = get_y(p);
        for (auto& p : get_adj_coord(x, y))
            if (is_onboard(p))
                m_adj[i].push_back(get_point(p.x, p.y));
        for (auto& p : get_diag_coord(x, y))
            if (is_onboard(p))
                m_diag[i].push_back(get_point(p.x, p.y));
    }
}

template<class P>
inline bool Geometry<P>::is_onboard(unsigned x, unsigned y) const
{
    return ! get_point(x, y).is_null();
}

template<class P>
bool Geometry<P>::is_onboard(CoordPoint p) const
{
    return p.is_onboard(m_width, m_height) && is_onboard(p.x, p.y);
}

#if LIBBOARDGAME_DEBUG

template<class P>
inline bool Geometry<P>::is_valid(Point p) const
{
    return ! p.is_null() && p.to_int() < get_range();
}

#endif

template<class P>
inline const string& Geometry<P>::to_string(Point p) const
{
    LIBBOARDGAME_ASSERT(p.to_int() < get_range());
    return m_string[p.to_int()];
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_GEOMETRY_H
