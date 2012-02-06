//-----------------------------------------------------------------------------
/** @file libboardgame_base/Point.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_POINT_H
#define LIBBOARDGAME_BASE_POINT_H

#include <array>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/Exception.h"
#include "libboardgame_util/NullElement.h"
#include "libboardgame_sys/Compiler.h"

namespace libboardgame_base {

using namespace std;
using namespace libboardgame_util;

//-----------------------------------------------------------------------------

/** @page libboardgame.boardgame.board_representation Board representation

For efficiency, points are represented using a one-dimensional value. Class
Point is a lightweight wrapper around an integer.

Many games allow different board sizes. Point::max_size defines the maximum
board size. For a given instantiation of Point with a given Point::max_size,
the mapping of integers to coordinates is independent of the actual board size,
which allows to read and write points without knowing the board size.

The integer value of the points increases from left to right and bottom to top.
The lower left corner of the board has the coordinates (0,0).

Point::null() has the index 0. It can be used when a special marker point is
needed (e.g. end of point list marker, or the special meaning "no point"). */

//-----------------------------------------------------------------------------

/** Coordinate on the board.
    Depending on the game, a point represents a field or intersection (in Go)
    on the board.
    @tparam W The maximum board width.
    @tparam H The maximum board height.
    @tparam I An unsigned integer type to store the point value
    @tparam S A class with functions to convert points from and to strings
    depending on the string representation of points in the game.
    @see @ref libboardgame.boardgame.board_representation */
template<unsigned int W, unsigned int H, typename I, class S>
class Point
{
public:
    typedef I IntType;

    typedef S StringRep;

    static const unsigned int max_width = W;

    static const unsigned int max_height = H;

    static_assert(W <= StringRep::max_width, "");

    static_assert(H <= StringRep::max_height, "");

    static_assert(numeric_limits<I>::is_integer, "");

    static_assert(! numeric_limits<I>::is_signed, "");

    friend class Direction;

    class Direction
    {
    public:
        friend class Point;

        Direction();

        static Direction left();

        static Direction right();

        static Direction up();

        static Direction down();

        static Direction up_left();

        static Direction up_right();

        static Direction down_left();

        static Direction down_right();

    private:
        static const int value_uninitialized = max_width + 1 + 1;

        int m_i;

        explicit Direction(int i);

        bool is_initialized() const;

        int to_int() const;
    };

    class InvalidString
        : public Exception
    {
    public:
        InvalidString(const string& s);
    };

    static const unsigned int max_onboard = max_width * max_height;

    /** Range of integer representation.
        Width times height on-board points and the null point. */
    static const unsigned int range = max_onboard + 1;

    /** Start of integer indices representing on-board points.
        All points with lower indices are off-board (but not all points
        between range_onboard_begin and range_onboard_end - 1 are on-board). */
    static const unsigned int range_onboard_begin = 1;

    /** End (exclusive) of integer indices representing on-board points.
        Users should not assume that range_onboard_end and range are equal to
        allow this class to be changed in the future or to replace this
        class by a different one in template classes that take the point class
        as a parameter. In different point representations (e.g. with an extra
        border of off-board points for loop unrolling in rectangular board
        geometries, range_onboard_end might be smaller than range. */
    static const unsigned int range_onboard_end = range;

    /** Special-purpose off-board point.
        This point is an off-board point with index 0. Among the use cases is
        an end marker for point lists or to initialize variables that carry a
        special meaning if no point has assigned to them yet. */
    static Point null();

    /** Parse a point from a string.
        @param s The string to parse
        @throws InvalidString */
    static Point from_string(const string& s);

    static void read(istream& in, Point& p);

    static bool is_x_coord(unsigned int i);

    static bool is_y_coord(unsigned int i);

    LIBBOARDGAME_FORCE_INLINE Point();

    Point(unsigned int x, unsigned int y);

    Point(const Point& p);

    explicit Point(unsigned int i);

    explicit Point(const string& s);

    bool operator==(const Point& p) const;

    bool operator!=(const Point& p) const;

    bool operator<(const Point& p) const;

    unsigned int get_x() const;

    unsigned int get_y() const;

    /** Check if point is on board on the largest possible board size.
        Allows to check that a point is potentially an on-board point without
        knowing the size of the board. Mainly used for assertions that do not
        have the board size context. */
    bool is_onboard() const;

    bool is_null() const;

    Point get_neighbor(Direction dir) const;

    Point get_left() const;

    Point get_right() const;

    Point get_up() const;

    Point get_down() const;

    Point get_up_left() const;

    Point get_up_right() const;

    Point get_down_left() const;

    Point get_down_right() const;

    /** Check if point is adjacent to another point. */
    bool is_adj(Point p) const;

    /** Check if point is an adjacent or diagonal neighbor of another point. */
    bool is_adj_diag(Point p) const;

    /** Return point as an integer between 0 and Point::range */
    unsigned int to_int() const;

    /** Return point as an integer between 0 and Point::range.
        @pre ! is_null() */
    unsigned int to_int_not_null() const;

    /** Convert to string.
        The string representation is compatible with
        @ref libboardgame_doc_gtp */
    string to_string() const;

    void write(ostream& out) const;

private:
    struct Precomputed
    {
        unsigned int x[Point::range];

        unsigned int y[Point::range];

        Precomputed();
    };

    static const Precomputed s_precomputed;

    static const IntType value_uninitialized = Point::range;

    static const IntType value_null = 0;

    IntType m_i;

    LIBBOARDGAME_FORCE_INLINE bool is_initialized() const;
};

template<unsigned int W, unsigned int H, typename I, class S>
Point<W,H,I,S>::Direction::Direction()
{
    m_i = value_uninitialized;
}

template<unsigned int W, unsigned int H, typename I, class S>
Point<W,H,I,S>::Direction::Direction(int i)
{
    m_i = i;
}

template<unsigned int W, unsigned int H, typename I, class S>
inline typename Point<W,H,I,S>::Direction Point<W,H,I,S>::Direction::down()
{
    return Direction(-static_cast<int>(max_width));
}

template<unsigned int W, unsigned int H, typename I, class S>
inline typename Point<W,H,I,S>::Direction Point<W,H,I,S>::Direction::down_left()
{
    return Direction(-static_cast<int>(max_width) - 1);
}

template<unsigned int W, unsigned int H, typename I, class S>
inline typename Point<W,H,I,S>::Direction
                                         Point<W,H,I,S>::Direction::down_right()
{
    return Direction(-static_cast<int>(max_width) + 1);
}

template<unsigned int W, unsigned int H, typename I, class S>
inline bool Point<W,H,I,S>::Direction::is_initialized() const
{
    return (m_i < value_uninitialized);
}

template<unsigned int W, unsigned int H, typename I, class S>
inline typename Point<W,H,I,S>::Direction Point<W,H,I,S>::Direction::left()
{
    return Direction(-1);
}

template<unsigned int W, unsigned int H, typename I, class S>
inline typename Point<W,H,I,S>::Direction Point<W,H,I,S>::Direction::right()
{
    return Direction(1);
}

template<unsigned int W, unsigned int H, typename I, class S>
inline int Point<W,H,I,S>::Direction::to_int() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i;
}

template<unsigned int W, unsigned int H, typename I, class S>
inline typename Point<W,H,I,S>::Direction Point<W,H,I,S>::Direction::up()
{
    return Direction(max_width);
}

template<unsigned int W, unsigned int H, typename I, class S>
inline typename Point<W,H,I,S>::Direction Point<W,H,I,S>::Direction::up_left()
{
    return Direction(max_width - 1);
}

template<unsigned int W, unsigned int H, typename I, class S>
inline typename Point<W,H,I,S>::Direction Point<W,H,I,S>::Direction::up_right()
{
    return Direction(max_width + 1);
}

template<unsigned int W, unsigned int H, typename I, class S>
Point<W,H,I,S>::InvalidString::InvalidString(const string& s)
    : Exception(format("Invalid point string '%1%'") % s)
{
}

template<unsigned int W, unsigned int H, typename I, class S>
Point<W,H,I,S>::Precomputed::Precomputed()
{
    // Make this a static assertion once GCC and MSVC support using
    // numeric_limits::max() in compile-time const expressions
    LIBBOARDGAME_ASSERT(Point::range <= numeric_limits<I>::max());

    for (unsigned int i = 1; i < Point::range; ++i)
    {
        y[i] = (i - 1) / max_width;
        x[i] = (i - 1) - y[i] * max_width;
    }
}

template<unsigned int W, unsigned int H, typename I, class S>
const typename Point<W,H,I,S>::Precomputed Point<W,H,I,S>::s_precomputed;

template<unsigned int W, unsigned int H, typename I, class S>
inline Point<W,H,I,S>::Point()
{
#if LIBBOARDGAME_DEBUG
    m_i = value_uninitialized;
#endif
}

template<unsigned int W, unsigned int H, typename I, class S>
inline Point<W,H,I,S>::Point(unsigned int i)
{
    LIBBOARDGAME_ASSERT(i < range);
    m_i = i;
}

template<unsigned int W, unsigned int H, typename I, class S>
inline Point<W,H,I,S>::Point(unsigned int x, unsigned int y)
{
    LIBBOARDGAME_ASSERT(is_x_coord(x));
    LIBBOARDGAME_ASSERT(is_y_coord(y));
    m_i = y * max_width + x + 1;
}

template<unsigned int W, unsigned int H, typename I, class S>
inline Point<W,H,I,S>::Point(const Point& p)
{
    m_i = p.m_i;
}

template<unsigned int W, unsigned int H, typename I, class S>
inline Point<W,H,I,S>::Point(const string& s)
{
    *this = from_string(s);
}

template<unsigned int W, unsigned int H, typename I, class S>
inline bool Point<W,H,I,S>::operator==(const Point& p) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(p.is_initialized());
    return (m_i == p.m_i);
}

template<unsigned int W, unsigned int H, typename I, class S>
inline bool Point<W,H,I,S>::operator!=(const Point& p) const
{
    return ! operator==(p);
}

template<unsigned int W, unsigned int H, typename I, class S>
inline bool Point<W,H,I,S>::operator<(const Point& p) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(p.is_initialized());
    return (m_i < p.m_i);
}

template<unsigned int W, unsigned int H, typename I, class S>
Point<W,H,I,S> Point<W,H,I,S>::from_string(const string& s)
{
    istringstream in(s);
    Point p;
    read(in, p);
    if (! in)
        throw InvalidString(s);
    // Check that no extra non-whitespace characters follow
    string extra;
    in >> extra;
    if (in)
        throw InvalidString(s);
    return p;
}

template<unsigned int W, unsigned int H, typename I, class S>
inline Point<W,H,I,S> Point<W,H,I,S>::get_down() const
{
    LIBBOARDGAME_ASSERT(get_y() > 0);
    return get_neighbor(Direction::down());
}

template<unsigned int W, unsigned int H, typename I, class S>
inline Point<W,H,I,S> Point<W,H,I,S>::get_down_left() const
{
    LIBBOARDGAME_ASSERT(get_y() > 0);
    LIBBOARDGAME_ASSERT(get_x() > 0);
    return get_neighbor(Direction::down_left());
}

template<unsigned int W, unsigned int H, typename I, class S>
inline Point<W,H,I,S> Point<W,H,I,S>::get_down_right() const
{
    LIBBOARDGAME_ASSERT(get_y() > 0);
    LIBBOARDGAME_ASSERT(get_x() < max_width - 1);
    return get_neighbor(Direction::down_right());
}

template<unsigned int W, unsigned int H, typename I, class S>
inline Point<W,H,I,S> Point<W,H,I,S>::get_left() const
{
    LIBBOARDGAME_ASSERT(get_x() > 0);
    return get_neighbor(Direction::left());
}

template<unsigned int W, unsigned int H, typename I, class S>
inline Point<W,H,I,S> Point<W,H,I,S>::get_neighbor(Direction dir) const
{
    LIBBOARDGAME_ASSERT(! is_null());
    return Point(m_i + dir.to_int());
}

template<unsigned int W, unsigned int H, typename I, class S>
inline Point<W,H,I,S> Point<W,H,I,S>::get_right() const
{
    LIBBOARDGAME_ASSERT(get_x() < max_width - 1);
    return get_neighbor(Direction::right());
}

template<unsigned int W, unsigned int H, typename I, class S>
inline Point<W,H,I,S> Point<W,H,I,S>::get_up() const
{
    LIBBOARDGAME_ASSERT(get_y() < max_height - 1);
    return get_neighbor(Direction::up());
}

template<unsigned int W, unsigned int H, typename I, class S>
inline Point<W,H,I,S> Point<W,H,I,S>::get_up_left() const
{
    LIBBOARDGAME_ASSERT(get_x() > 0);
    LIBBOARDGAME_ASSERT(get_y() < max_height - 1);
    return get_neighbor(Direction::up_left());
}

template<unsigned int W, unsigned int H, typename I, class S>
inline Point<W,H,I,S> Point<W,H,I,S>::get_up_right() const
{
    LIBBOARDGAME_ASSERT(get_x() < max_width - 1);
    LIBBOARDGAME_ASSERT(get_y() < max_height - 1);
    return get_neighbor(Direction::up_right());
}

template<unsigned int W, unsigned int H, typename I, class S>
inline unsigned int Point<W,H,I,S>::get_x() const
{
    LIBBOARDGAME_ASSERT(! is_null());
    return s_precomputed.x[m_i];
}

template<unsigned int W, unsigned int H, typename I, class S>
inline unsigned int Point<W,H,I,S>::get_y() const
{
    LIBBOARDGAME_ASSERT(! is_null());
    return s_precomputed.y[m_i];
}

template<unsigned int W, unsigned int H, typename I, class S>
inline bool Point<W,H,I,S>::is_adj(Point p) const
{
    int d = m_i - p.m_i;
    d = abs(d);
    return (d == max_width || d == 1);
}

template<unsigned int W, unsigned int H, typename I, class S>
inline bool Point<W,H,I,S>::is_adj_diag(Point p) const
{
    int d = m_i - p.m_i;
    d = abs(d);
    return ((d >= static_cast<int>(max_width - 1)
             && d <= static_cast<int>(max_width + 1))
            || d == 1);
}

template<unsigned int W, unsigned int H, typename I, class S>
inline bool Point<W,H,I,S>::is_initialized() const
{
    return m_i < value_uninitialized;
}

template<unsigned int W, unsigned int H, typename I, class S>
inline bool Point<W,H,I,S>::is_onboard() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return ! is_null();
}

template<unsigned int W, unsigned int H, typename I, class S>
inline bool Point<W,H,I,S>::is_null() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i == value_null;
}

template<unsigned int W, unsigned int H, typename I, class S>
inline bool Point<W,H,I,S>::is_x_coord(unsigned int i)
{
    return i < max_width;
}

template<unsigned int W, unsigned int H, typename I, class S>
inline bool Point<W,H,I,S>::is_y_coord(unsigned int i)
{
    return i < max_height;
}

template<unsigned int W, unsigned int H, typename I, class S>
inline Point<W,H,I,S> Point<W,H,I,S>::null()
{
    return Point(value_null);
}

template<unsigned int W, unsigned int H, typename I, class S>
void Point<W,H,I,S>::read(istream& in, Point<W,H,I,S>& p)
{
    unsigned int x;
    unsigned int y;
    if (StringRep::read(in, max_width, max_height, x, y))
        p = Point(x, y);
    else
        in.setstate(ios::failbit);
}

template<unsigned int W, unsigned int H, typename I, class S>
inline unsigned int Point<W,H,I,S>::to_int() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i;
}

template<unsigned int W, unsigned int H, typename I, class S>
inline unsigned int Point<W,H,I,S>::to_int_not_null() const
{
    LIBBOARDGAME_ASSERT(! is_null());
    return m_i;
}

template<unsigned int W, unsigned int H, typename I, class S>
inline string Point<W,H,I,S>::to_string() const
{
    ostringstream s;
    write(s);
    return s.str();
}

template<unsigned int W, unsigned int H, typename I, class S>
void Point<W,H,I,S>::write(ostream& out) const
{
    if (is_null())
        out << "NULL";
    else
        StringRep::write(out, get_x(), get_y());
}

//-----------------------------------------------------------------------------

template<unsigned int W, unsigned int H, typename I, class S>
inline ostream& operator<<(ostream& out, const Point<W,H,I,S>& p)
{
    p.write(out);
    return out;
}

/** Read point from input stream.
    The function guarantees to support the point representation used by
    @ref libboardgame_doc_gtp, so it can be used for parsing GTP arguments. */
template<unsigned int W, unsigned int H, typename I, class S>
inline istream& operator>>(istream& in, Point<W,H,I,S>& p)
{
    Point<W,H,I,S>::read(in, p);
    return in;
}

//-----------------------------------------------------------------------------

} // namespace boardgame_libboardgame

//-----------------------------------------------------------------------------

namespace std {

template<unsigned int W, unsigned int H, typename I, class S>
struct hash<libboardgame_base::Point<W,H,I,S>>
    : public unary_function<libboardgame_base::Point<W,H,I,S>, size_t>
{
    size_t operator()(const libboardgame_base::Point<W,H,I,S>& p) const
    {
        return p.to_int();
    }
};

} // namespace std

//-----------------------------------------------------------------------------

namespace libboardgame_util {

template<unsigned int W, unsigned int H, typename I, class S>
inline bool is_null(const libboardgame_base::Point<W,H,I,S>& p)
{
    return p.is_null();
}

template<unsigned int W, unsigned int H, typename I, class S>
inline void set_null(libboardgame_base::Point<W,H,I,S>& p)
{
    p = libboardgame_base::Point<W,H,I,S>::null();
}

} // namespace libboardgame_util

//-----------------------------------------------------------------------------

#endif // LIBBOARDGAME_BASE_POINT_H
