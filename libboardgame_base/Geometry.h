//-----------------------------------------------------------------------------
/** @file libboardgame_base/Geometry.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_GEOMETRY_H
#define LIBBOARDGAME_BASE_GEOMETRY_H

#include <memory>
#include <sstream>
#include "ArrayList.h"
#include "CoordPoint.h"
#include "StringRep.h"

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

/** %Geometry data of a board with a given size.
    This class is a base class that uses virtual functions in its constructor
    that can restrict the shape of the board to a subset of the rectangle
    and/or to define different definitions of adjacent and diagonal neighbors
    of a point for geometries that are not rectangular grids.
    @tparam P An instantiation of Point (or compatible class) */
template<class P>
class Geometry
{
public:
    using Point = P;

    using IntType = typename Point::IntType;

    static constexpr unsigned max_adj = 4;

    static constexpr unsigned max_diag = 11;

    /** On-board adjacent neighbors of a point. */
    using AdjList = ArrayList<Point, max_adj, unsigned short>;

    /** On-board diagonal neighbors of a point
        Currently supports up to 11 diagonal points as used on boards
        for GembloQ. */
    using DiagList = ArrayList<Point, max_diag, unsigned short>;

    /** Adjacent neighbors of a coordinate. */
    using AdjCoordList = ArrayList<CoordPoint, max_adj>;

    /** Diagonal neighbors of a coordinate. */
    using DiagCoordList = ArrayList<CoordPoint, max_diag>;

    class Iterator
    {
    public:
        explicit Iterator(IntType i) { m_i = i; }

        bool operator==(Iterator it) const { return m_i == it.m_i; }

        bool operator!=(Iterator it) const { return m_i != it.m_i; }

        void operator++() { ++m_i; }

        Point operator*() const { return Point(m_i); }

    private:
        IntType m_i;
    };

    virtual ~Geometry();

    /** Get points that share an edge with this point. */
    virtual AdjCoordList get_adj_coord(int x, int y) const = 0;

    /** Get points that share a corner but not an edge with this point.
        The order does not matter logically but it is better to put far away
        points first because BoardConst uses the forbidden status of the first
        points during move generation and far away points can reject more
        moves. */
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

    Iterator begin() const { return Iterator(0); }

    Iterator end() const { return Iterator(m_range); }

    unsigned get_point_type(CoordPoint p) const;

    unsigned get_point_type(Point p) const;

    bool is_onboard(CoordPoint p) const;

    bool is_onboard(unsigned x, unsigned y) const;

    bool is_onboard(int x, int y) const { return is_onboard({x, y}); }

    /** Return the point at a given coordinate.
        @pre x < get_width()
        @pre y < get_height()
        @return The point or Point::null() if this coordinates are
        off-board. */
    Point get_point(unsigned x, unsigned y) const;

    /** Return the point at a given coordinate.
        @return The point or Point::null() if this coordinates are
        off-board. */
    Point get_point(int x, int y) const;

    unsigned get_width() const { return m_width; }

    unsigned get_height() const { return m_height; }

    /** Get range used for onboard points. */
    IntType get_range() const { return m_range; }

    unsigned get_x(Point p) const;

    unsigned get_y(Point p) const;

    bool from_string(string::const_iterator begin, string::const_iterator end,
                     Point& p) const;

    const string& to_string(Point p) const;

    const AdjList& get_adj(Point p) const;

    const DiagList& get_diag(Point p) const;

protected:
    explicit Geometry(
            unique_ptr<StringRep> string_rep = make_unique<StdStringRep>());

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
    IntType m_range;

    AdjList m_adj[Point::range_onboard];

    DiagList m_diag[Point::range_onboard];

    Point m_points[Point::max_width][Point::max_height];

    unique_ptr<StringRep> m_string_rep;

    unsigned m_width;

    unsigned m_height;

    unsigned m_x[Point::range_onboard];

    unsigned m_y[Point::range_onboard];

    unsigned m_point_type[Point::range_onboard];

    string m_string[Point::range];

#ifdef LIBBOARDGAME_DEBUG
    bool is_valid(Point p) const { return p.to_int() < m_range; }
#endif
};


template<class P>
Geometry<P>::Geometry(unique_ptr<StringRep> string_rep)
    : m_string_rep(std::move(string_rep))
{ }

template<class P>
Geometry<P>::~Geometry() = default; // Non-inline to avoid GCC -Winline warning

template<class P>
bool Geometry<P>::from_string(string::const_iterator begin,
                              string::const_iterator end, Point& p) const
{
    unsigned x;
    unsigned y;
    if (! m_string_rep->read(begin, end, m_width, m_height, x, y)
            || ! is_onboard(x, y))
        return false;
    p = get_point(x, y);
    return true;
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
inline auto Geometry<P>::get_point(unsigned x, unsigned y) const -> Point
{
    LIBBOARDGAME_ASSERT(x < m_width);
    LIBBOARDGAME_ASSERT(y < m_height);
    return m_points[x][y];
}

template<class P>
inline auto Geometry<P>::get_point(int x, int y) const -> Point
{
    if (x < 0 || static_cast<unsigned>(x) >= m_width
            || y < 0 || static_cast<unsigned>(y) >= m_height)
        return Point::null();
    return m_points[x][y];
}

template<class P>
inline unsigned Geometry<P>::get_point_type(Point p) const
{
    LIBBOARDGAME_ASSERT(is_valid(p));
    return m_point_type[p.to_int()];
}

template<class P>
inline unsigned Geometry<P>::get_point_type(CoordPoint p) const
{
    return get_point_type(p.x, p.y);
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
    IntType n = 0;
    ostringstream ostr;
    for (unsigned y = 0; y < height; ++y)
        for (unsigned x = 0; x < width; ++x)
            if (init_is_onboard(x, y))
            {
                m_points[x][y] = Point(n);
                m_x[n] = x;
                m_y[n] = y;
                ostr.str("");
                m_string_rep->write(ostr, x, y, width, height);
                m_string[n] = ostr.str();
                ++n;
            }
            else
                m_points[x][y] = Point::null();
    m_range = n;
    for (IntType i = 0; i < n; ++i)
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
        m_point_type[i] = get_point_type(x, y);
    }
}

template<class P>
bool Geometry<P>::is_onboard(unsigned x, unsigned y) const
{
    return x < m_width && y < m_height && ! get_point(x, y).is_null();
}

template<class P>
bool Geometry<P>::is_onboard(CoordPoint p) const
{
    return p.is_onboard(m_width, m_height) && ! get_point(p.x, p.y).is_null();
}

template<class P>
inline const string& Geometry<P>::to_string(Point p) const
{
    LIBBOARDGAME_ASSERT(p.to_int() < m_range);
    return m_string[p.to_int()];
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_GEOMETRY_H
