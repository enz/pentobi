//-----------------------------------------------------------------------------
/** @file libboardgame_base/Point.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_POINT_H
#define LIBBOARDGAME_BASE_POINT_H

#include <iosfwd>
#include <limits>
#include <sstream>
#include <string>
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/Exception.h"
#include "libboardgame_sys/Compiler.h"

namespace libboardgame_base {

using namespace std;
using namespace libboardgame_util;

//-----------------------------------------------------------------------------

/** @page libboardgame.boardgame.board_representation Board representation

For efficiency, points are represented using a one-dimensional value. Class
Point is a lightweight wrapper around an integer.

On a given board size with rectangular geometry, points are layed out in a
continuous range to avoid unused gaps if the board size is smaller than the
maximum board size that could hurt the performance of the CPU cache.
Therefore, the interpretation of a point depends on the width of the current
board and many member functions need a width argument.

The integer value of the points increases from left to right and top to bottom.
The top left corner of the board has the coordinates (0,0).
Point::null() has the index 0. It can be used when a special marker point is
needed (e.g. end of point list marker, or the special meaning "no point"). */

//-----------------------------------------------------------------------------

/** Coordinate on the board.
    Depending on the game, a point represents a field or intersection (in Go)
    on the board.
    @tparam M The maximum number of points (excluding the null point).
    @tparam I An unsigned integer type to store the point value
    @tparam S A class with functions to convert points from and to strings
    depending on the string representation of points in the game.
    @see @ref libboardgame.boardgame.board_representation */
template<unsigned M, typename I, class S>
class Point
{
public:
    typedef I IntType;

    typedef S StringRep;

    static const unsigned max_onboard = M;

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

        static Direction up(unsigned width);

        static Direction down(unsigned width);

        static Direction up_left(unsigned width);

        static Direction up_right(unsigned width);

        static Direction down_left(unsigned width);

        static Direction down_right(unsigned width);

    private:
        static const int value_uninitialized = max_onboard + 1;

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

    /** Range of integer representation.
        Width times height on-board points and the null point. */
    static const unsigned range = max_onboard + 1;

    static unsigned get_range(unsigned width, unsigned height);

    /** Special-purpose off-board point.
        This point is an off-board point with index 0. Among the use cases is
        an end marker for point lists or to initialize variables that carry a
        special meaning if no point has been assigned to them yet. */
    static Point null();

    /** Parse a point from a string.
        @param s The string to parse
        @throws InvalidString */
    static Point from_string(const string& s, unsigned width, unsigned height);

    static void read(istream& in, Point& p, unsigned width, unsigned height);

    LIBBOARDGAME_FORCE_INLINE Point();

    Point(unsigned x, unsigned y, unsigned width);

    explicit Point(unsigned i);

    Point(const string& s, unsigned width, unsigned height);

    bool operator==(const Point& p) const;

    bool operator!=(const Point& p) const;

    bool operator<(const Point& p) const;

    unsigned get_x(unsigned width) const;

    unsigned get_y(unsigned width) const;

    /** Check if point is on board on the largest possible board size.
        Allows to check that a point is potentially an on-board point without
        knowing the size of the board. Mainly used for assertions that do not
        have the board size context. */
    bool is_onboard() const;

    bool is_null() const;

    Point get_neighbor(Direction dir) const;

    Point get_left() const;

    Point get_right() const;

    Point get_up(unsigned width) const;

    Point get_down(unsigned width) const;

    Point get_up_left(unsigned width) const;

    Point get_up_right(unsigned width) const;

    Point get_down_left(unsigned width) const;

    Point get_down_right(unsigned width) const;

    /** Return point as an integer between 0 and Point::range */
    unsigned to_int() const;

    /** Return point as an integer between 0 and Point::range.
        @pre ! is_null() */
    unsigned to_int_not_null() const;

    /** Convert to string. */
    string to_string(unsigned width, unsigned height) const;

    void write(ostream& out, unsigned width, unsigned height) const;

private:
    static const IntType value_uninitialized = Point::range;

    static const IntType value_null = 0;

    IntType m_i;

    LIBBOARDGAME_FORCE_INLINE bool is_initialized() const;
};

template<unsigned M, typename I, class S>
Point<M, I, S>::Direction::Direction()
{
    m_i = value_uninitialized;
}

template<unsigned M, typename I, class S>
Point<M, I, S>::Direction::Direction(int i)
{
    m_i = i;
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::Direction::down(unsigned width) -> Direction
{
    return Direction(width);
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::Direction::down_left(unsigned width) -> Direction
{
    return Direction(width - 1);
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::Direction::down_right(unsigned width)
    -> Direction
{
    return Direction(width + 1);
}

template<unsigned M, typename I, class S>
inline bool Point<M, I, S>::Direction::is_initialized() const
{
    return (m_i < value_uninitialized);
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::Direction::left() -> Direction
{
    return Direction(-1);
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::Direction::right() -> Direction
{
    return Direction(1);
}

template<unsigned M, typename I, class S>
inline int Point<M, I, S>::Direction::to_int() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i;
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::Direction::up(unsigned width) -> Direction
{
    return Direction(-static_cast<int>(width));
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::Direction::up_left(unsigned width)
    -> Direction
{
    return Direction(-static_cast<int>(width) - 1);
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::Direction::up_right(unsigned width)
    -> Direction
{
    return Direction(-static_cast<int>(width) + 1);
}


template<unsigned M, typename I, class S>
Point<M, I, S>::InvalidString::InvalidString(const string& s)
    : Exception("Invalid point string '" + s + "'")
{
}


template<unsigned M, typename I, class S>
inline Point<M, I, S>::Point()
{
#if LIBBOARDGAME_DEBUG
    m_i = value_uninitialized;
#endif
}

template<unsigned M, typename I, class S>
inline Point<M, I, S>::Point(unsigned i)
{
    LIBBOARDGAME_ASSERT(i < range);
    m_i = static_cast<I>(i);
}

template<unsigned M, typename I, class S>
inline Point<M, I, S>::Point(unsigned x, unsigned y, unsigned width)
    : Point(static_cast<I>(y * width + x + 1))
{ }

template<unsigned M, typename I, class S>
inline Point<M, I, S>::Point(const string& s, unsigned width, unsigned height)
{
    *this = from_string(s, width, height);
}

template<unsigned M, typename I, class S>
inline bool Point<M, I, S>::operator==(const Point& p) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(p.is_initialized());
    return (m_i == p.m_i);
}

template<unsigned M, typename I, class S>
inline bool Point<M, I, S>::operator!=(const Point& p) const
{
    return ! operator==(p);
}

template<unsigned M, typename I, class S>
inline bool Point<M, I, S>::operator<(const Point& p) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(p.is_initialized());
    return (m_i < p.m_i);
}

template<unsigned M, typename I, class S>
auto Point<M, I, S>::from_string(const string& s, unsigned width,
                                 unsigned height) -> Point
{
    istringstream in(s);
    Point p = Point::null(); // Initialize to avoid compiler warning
    read(in, p, width, height);
    if (! in)
        throw InvalidString(s);
    // Check that no extra non-whitespace characters follow
    string extra;
    in >> extra;
    if (in)
        throw InvalidString(s);
    return p;
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::get_down(unsigned width) const -> Point
{
    return get_neighbor(Direction::down(width));
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::get_down_left(unsigned width) const -> Point
{
    return get_neighbor(Direction::down_left(width));
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::get_down_right(unsigned width) const -> Point
{
    return get_neighbor(Direction::down_right(width));
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::get_left() const -> Point
{
    return get_neighbor(Direction::left());
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::get_neighbor(Direction dir) const -> Point
{
    LIBBOARDGAME_ASSERT(! is_null());
    return Point(m_i + dir.to_int());
}

template<unsigned M, typename I, class S>
unsigned Point<M, I, S>::get_range(unsigned width, unsigned height)
{
    return width * height + 1;
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::get_right() const -> Point
{
    return get_neighbor(Direction::right());
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::get_up(unsigned width) const -> Point
{
    return get_neighbor(Direction::up(width));
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::get_up_left(unsigned width) const -> Point
{
    return get_neighbor(Direction::up_left(width));
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::get_up_right(unsigned width) const -> Point
{
    return get_neighbor(Direction::up_right(width));
}

template<unsigned M, typename I, class S>
unsigned Point<M, I, S>::get_x(unsigned width) const
{
    LIBBOARDGAME_ASSERT(! is_null());
    return (m_i - 1) - get_y(width) * width;
}

template<unsigned M, typename I, class S>
unsigned Point<M, I, S>::get_y(unsigned width) const
{
    LIBBOARDGAME_ASSERT(! is_null());
    return (m_i - 1) / width;
}

template<unsigned M, typename I, class S>
inline bool Point<M, I, S>::is_initialized() const
{
    return m_i < value_uninitialized;
}

template<unsigned M, typename I, class S>
inline bool Point<M, I, S>::is_onboard() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return ! is_null();
}

template<unsigned M, typename I, class S>
inline bool Point<M, I, S>::is_null() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i == value_null;
}

template<unsigned M, typename I, class S>
inline auto Point<M, I, S>::null() -> Point
{
    return Point(value_null);
}

template<unsigned M, typename I, class S>
void Point<M, I, S>::read(istream& in, Point& p, unsigned width,
                          unsigned height)
{
    unsigned x;
    unsigned y;
    if (StringRep::read(in, width, height, x, y))
        p = Point(x, y, width);
    else
        in.setstate(ios::failbit);
}

template<unsigned M, typename I, class S>
inline unsigned Point<M, I, S>::to_int() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i;
}

template<unsigned M, typename I, class S>
inline unsigned Point<M, I, S>::to_int_not_null() const
{
    LIBBOARDGAME_ASSERT(! is_null());
    return m_i;
}

template<unsigned M, typename I, class S>
inline string Point<M, I, S>::to_string(unsigned width, unsigned height) const
{
    ostringstream s;
    write(s, width, height);
    return s.str();
}

template<unsigned M, typename I, class S>
void Point<M, I, S>::write(ostream& out, unsigned width, unsigned height) const
{
    if (is_null())
        out << "NULL";
    else
        StringRep::write(out, get_x(width), get_y(width), width, height);
}

//-----------------------------------------------------------------------------

} // namespace boardgame_libboardgame

#endif // LIBBOARDGAME_BASE_POINT_H
