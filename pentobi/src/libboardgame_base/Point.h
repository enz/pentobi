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
#include "libboardgame_util/ArrayList.h"
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
Point is a lightweight wrapper around an integer. Around the board is a row of
border points.

Many games allow different board sizes. Point::max_size defines the maximum
board size. For a given instantiation of Point with a given Point::max_size,
the mapping of integers to coordinates is independent of the actual board size,
which allows to read and write points without knowing the board size and makes
the integer differences between neighbors compile-time constants. On boards
smaller than Point::max_size the status of points that are not adjacent or
diagonal to any on-board point is undefined.

The integer value of the points increases from left to right and bottom to top.
The lower left corner of the board has the coordinates (0,0). The following
table shows the point integers for Point::max_size 19.

<pre>
19|401 402 403 404 405 406 407 408 409 410 411 412 413 414 415 416 417 418 419
18|380 381 382 383 384 385 386 387 388 389 390 391 392 393 394 395 396 397 398
17|359 360 361 362 363 364 365 366 367 368 369 370 371 372 373 374 375 376 377
16|338 339 340 341 342 343 344 345 346 347 348 349 350 351 352 353 354 355 356
15|317 318 319 320 321 322 323 324 325 326 327 328 329 330 331 332 333 334 325
14|296 297 298 299 300 301 302 303 304 305 306 307 308 309 310 311 312 313 314
13|275 276 277 278 279 280 281 282 283 284 285 286 287 288 289 290 291 292 293
12|254 255 256 257 258 259 260 261 262 263 264 265 266 267 268 269 270 271 272
11|233 234 235 236 237 238 239 240 241 242 243 244 245 246 247 248 249 250 251
10|212 213 214 215 216 217 218 219 220 221 222 223 224 225 226 227 228 229 230
 9|191 192 193 194 195 196 197 198 199 200 201 202 203 204 205 206 207 208 209
 8|170 171 172 173 174 175 176 177 178 179 180 181 182 183 184 185 186 187 188
 7|149 150 151 152 153 154 155 156 157 158 159 160 161 162 163 164 165 166 167
 6|128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143 144 145 146
 5|107 108 109 110 111 112 113 114 115 116 117 118 119 120 121 122 123 124 125
 4| 86  87  88  89  90  91  92  93  94  95  96  97  98  99 100 101 102 103 104
 3| 65  66  67  68  69  70  71  72  73  74  75  76  77  78  79  80  81  82  83
 2| 44  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62
 1| 23  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39  40  41
  +---------------------------------------------------------------------------
    A   B   C   D   E   F   G   H   J   K   L   M   N   O   P   Q   R   S   T
</pre>

Point::null() has the index 0 and cannot be reached in simple flood-filling
algorithms that check only the four adjacent points or four diagonal points of
an on-board point. Therefore, it can be used when a special marker point is
needed (e.g. end of point list marker, or the special meaning "no point"). */

//-----------------------------------------------------------------------------

/** Coordinate on board or move.
    @tparam S The maximum board size.
    @tparam T A class with functions to convert points from and to strings
    depending on the string representation of points in the game.
    @see @ref libboardgame.boardgame.board_representation */
template<unsigned int S, class T>
class Point
{
public:
    typedef T StringRep;

    static const unsigned int max_size = S;

    static_assert(S <= StringRep::max_size,
      "Template argument S for Point exceeds limit of string representation");

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

        /** Get an adjacent direction by index.
            @param i The index 0..3 in an enumeration of all four adjacent
            directions. */
        static Direction get_enum_adj(unsigned int i);

        /** Get a diagonal direction by index.
            @param i The index 0..3 in an enumeration of all four diagonal
            directions. */
        static Direction get_enum_diag(unsigned int i);

        /** Get the index of the inverse diagonal direction.
            @param i The index of the diagonal direction as in get_enum_diag()
            @return The index of the inverse direction of i as in
            get_enum_diag() */
        static unsigned int get_index_diag_inv(unsigned int i);

    private:
        static const int value_uninitialized = max_size + 2 + 1 + 1;

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

    static const unsigned int range = (max_size + 2) * (max_size + 2) + 1;

    /** Start of integer indices representing on-board points.
        All points with lower indices are off-board (but not all points
        between range_onboard_begin and range_onboard_end - 1 are on-board). */
    static const unsigned int range_onboard_begin =
        (max_size + 2) + 1 + 1;

    /** End (exclusive) of integer indices representing on-board points.
        All points with larger or equal indices are off-board (but not all
        points between range_onboard_begin and range_onboard_end - 1 are
        on-board). */
    static const unsigned int range_onboard_end =
        (max_size + 2) * (max_size + 1);

    static const unsigned int max_onboard = max_size * max_size;

    /** Special-purpose off-board point.
        This point is an off-board point with index 0. It has the property that
        it cannot be reached in typical flood filling algorithms by applying
        left(), right(), up(), or down() from an on-board point. (However, it
        can be reached when checking diagonals of on-board points!) Among the
        potential use cases is an end marker for point lists or to initialize
        variables that carry a special meaning if no point has assigned to them
        yet. */
    static Point null();

    /** Parse a point from a string.
        @param s The string to parse
        @throws InvalidString */
    static Point from_string(const string& s);

    static void read(istream& in, Point& p);

    static bool is_coord(unsigned int i);

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

    /** Check if point is on board on a given board size. */
    bool is_onboard(unsigned int sz) const;

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
        unsigned int min_onboard_size[Point::range];

        unsigned int x[Point::range];

        unsigned int y[Point::range];

        ArrayList<Direction, 4> enum_adj;

        ArrayList<Direction, 4> enum_diag;

        ArrayList<unsigned int, 4> index_diag_inv;

        Precomputed();
    };

    static const Precomputed s_precomputed;

    static const unsigned int value_uninitialized = Point::range;

    static const unsigned int value_null = 0;

    unsigned int m_i;

    LIBBOARDGAME_FORCE_INLINE bool is_initialized() const;
};

template<unsigned int S, class T>
Point<S, T>::Direction::Direction()
{
    m_i = value_uninitialized;
}

template<unsigned int S, class T>
Point<S, T>::Direction::Direction(int i)
{
    m_i = i;
}

template<unsigned int S, class T>
inline typename Point<S, T>::Direction Point<S, T>::Direction::down()
{
    return Direction(-static_cast<int>(max_size) - 2);
}

template<unsigned int S, class T>
inline typename Point<S, T>::Direction Point<S, T>::Direction::down_left()
{
    return Direction(-static_cast<int>(max_size) - 2 - 1);
}

template<unsigned int S, class T>
inline typename Point<S, T>::Direction Point<S, T>::Direction::down_right()
{
    return Direction(-static_cast<int>(max_size) - 2 + 1);
}

template<unsigned int S, class T>
typename Point<S, T>::Direction
                           Point<S, T>::Direction::get_enum_adj(unsigned int i)
{
    return Point::s_precomputed.enum_adj[i];
}

template<unsigned int S, class T>
typename Point<S, T>::Direction
                          Point<S, T>::Direction::get_enum_diag(unsigned int i)
{
    return Point::s_precomputed.enum_diag[i];
}

template<unsigned int S, class T>
unsigned int Point<S, T>::Direction::get_index_diag_inv(unsigned int i)
{
    return Point::s_precomputed.index_diag_inv[i];
}

template<unsigned int S, class T>
inline bool Point<S, T>::Direction::is_initialized() const
{
    return (m_i < value_uninitialized);
}

template<unsigned int S, class T>
inline typename Point<S, T>::Direction Point<S, T>::Direction::left()
{
    return Direction(-1);
}

template<unsigned int S, class T>
inline typename Point<S, T>::Direction Point<S, T>::Direction::right()
{
    return Direction(1);
}

template<unsigned int S, class T>
inline int Point<S, T>::Direction::to_int() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i;
}

template<unsigned int S, class T>
inline typename Point<S, T>::Direction Point<S, T>::Direction::up()
{
    return Direction(max_size + 2);
}

template<unsigned int S, class T>
inline typename Point<S, T>::Direction Point<S, T>::Direction::up_left()
{
    return Direction(max_size + 2 - 1);
}

template<unsigned int S, class T>
inline typename Point<S, T>::Direction Point<S, T>::Direction::up_right()
{
    return Direction(max_size + 2 + 1);
}

template<unsigned int S, class T>
Point<S, T>::InvalidString::InvalidString(const string& s)
    : Exception(format("Invalid point string '%1%'") % s)
{
}

template<unsigned int S, class T>
Point<S, T>::Precomputed::Precomputed()
{
    enum_adj.push_back(Direction::down());
    enum_adj.push_back(Direction::left());
    enum_adj.push_back(Direction::right());
    enum_adj.push_back(Direction::up());
    enum_diag.push_back(Direction::down_left());
    enum_diag.push_back(Direction::down_right());
    enum_diag.push_back(Direction::up_left());
    enum_diag.push_back(Direction::up_right());
    index_diag_inv.push_back(3);
    index_diag_inv.push_back(2);
    index_diag_inv.push_back(1);
    index_diag_inv.push_back(0);
    unsigned int infinity = max_size + 1;
    min_onboard_size[0] = infinity;
    for (unsigned int i = 1; i < Point::range; ++i)
    {
        Point p(i);
        unsigned int raw_y = (i - 1) / (max_size + 2);
        if (raw_y == 0 || raw_y > max_size)
        {
            min_onboard_size[i] = infinity;
            continue;
        }
        min_onboard_size[i] = raw_y;
        unsigned int raw_x = (i - 1) - raw_y * (max_size + 2);
        if (raw_x == 0 || raw_x > max_size)
        {
            min_onboard_size[i] = infinity;
            continue;
        }
        min_onboard_size[i] = max(min_onboard_size[i], raw_x);
        x[i] = raw_x - 1;
        y[i] = raw_y - 1;
    }
}

template<unsigned int S, class T>
const typename Point<S, T>::Precomputed Point<S, T>::s_precomputed;

template<unsigned int S, class T>
inline Point<S, T>::Point()
{
#if LIBBOARDGAME_DEBUG
    m_i = value_uninitialized;
#endif
}

template<unsigned int S, class T>
inline Point<S, T>::Point(unsigned int i)
{
    LIBBOARDGAME_ASSERT(i < range);
    m_i = i;
}

template<unsigned int S, class T>
inline Point<S, T>::Point(unsigned int x, unsigned int y)
{
    LIBBOARDGAME_ASSERT(is_coord(x));
    LIBBOARDGAME_ASSERT(is_coord(y));
    m_i = (y + 1) * (max_size + 2) + x + 2;
}

template<unsigned int S, class T>
inline Point<S, T>::Point(const Point& p)
{
    m_i = p.m_i;
}

template<unsigned int S, class T>
inline Point<S, T>::Point(const string& s)
{
    *this = from_string(s);
}

template<unsigned int S, class T>
inline bool Point<S, T>::operator==(const Point& p) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(p.is_initialized());
    return (m_i == p.m_i);
}

template<unsigned int S, class T>
inline bool Point<S, T>::operator!=(const Point& p) const
{
    return ! operator==(p);
}

template<unsigned int S, class T>
inline bool Point<S, T>::operator<(const Point& p) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(p.is_initialized());
    return (m_i < p.m_i);
}

template<unsigned int S, class T>
Point<S, T> Point<S, T>::from_string(const string& s)
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

template<unsigned int S, class T>
inline Point<S, T> Point<S, T>::get_down() const
{
    return get_neighbor(Direction::down());
}

template<unsigned int S, class T>
inline Point<S, T> Point<S, T>::get_down_left() const
{
    return get_neighbor(Direction::down_left());
}

template<unsigned int S, class T>
inline Point<S, T> Point<S, T>::get_down_right() const
{
    return get_neighbor(Direction::down_right());
}

template<unsigned int S, class T>
inline Point<S, T> Point<S, T>::get_left() const
{
    return get_neighbor(Direction::left());
}

template<unsigned int S, class T>
inline Point<S, T> Point<S, T>::get_neighbor(Direction dir) const
{
    LIBBOARDGAME_ASSERT(is_onboard());
    return Point(m_i + dir.to_int());
}

template<unsigned int S, class T>
inline Point<S, T> Point<S, T>::get_right() const
{
    return get_neighbor(Direction::right());
}

template<unsigned int S, class T>
inline Point<S, T> Point<S, T>::get_up() const
{
    return get_neighbor(Direction::up());
}

template<unsigned int S, class T>
inline Point<S, T> Point<S, T>::get_up_left() const
{
    return get_neighbor(Direction::up_left());
}

template<unsigned int S, class T>
inline Point<S, T> Point<S, T>::get_up_right() const
{
    return get_neighbor(Direction::up_right());
}

template<unsigned int S, class T>
inline unsigned int Point<S, T>::get_x() const
{
    LIBBOARDGAME_ASSERT(is_onboard());
    return s_precomputed.x[m_i];
}

template<unsigned int S, class T>
inline unsigned int Point<S, T>::get_y() const
{
    LIBBOARDGAME_ASSERT(is_onboard());
    return s_precomputed.y[m_i];
}

template<unsigned int S, class T>
inline bool Point<S, T>::is_adj(Point p) const
{
    int d = m_i - p.m_i;
    d = abs(d);
    return (d == max_size + 2 || d == 1);
}

template<unsigned int S, class T>
inline bool Point<S, T>::is_adj_diag(Point p) const
{
    int d = m_i - p.m_i;
    d = abs(d);
    return ((d >= static_cast<int>(max_size + 2 - 1)
             && d <= static_cast<int>(max_size + 2 + 1))
            || d == 1);
}

template<unsigned int S, class T>
inline bool Point<S, T>::is_coord(unsigned int i)
{
    return i < max_size;
}

template<unsigned int S, class T>
inline bool Point<S, T>::is_initialized() const
{
    return m_i < value_uninitialized;
}

template<unsigned int S, class T>
inline bool Point<S, T>::is_onboard() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return s_precomputed.min_onboard_size[m_i] <= max_size;
}

template<unsigned int S, class T>
inline bool Point<S, T>::is_onboard(unsigned int sz) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return s_precomputed.min_onboard_size[m_i] <= sz;
}

template<unsigned int S, class T>
inline bool Point<S, T>::is_null() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i == value_null;
}

template<unsigned int S, class T>
inline Point<S, T> Point<S, T>::null()
{
    return Point(value_null);
}

template<unsigned int S, class T>
void Point<S, T>::read(istream& in, Point<S, T>& p)
{
    unsigned int x;
    unsigned int y;
    if (StringRep::read(in, max_size, x, y))
        p = Point(x, y);
    else
        in.setstate(ios::failbit);
}

template<unsigned int S, class T>
inline unsigned int Point<S, T>::to_int() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i;
}

template<unsigned int S, class T>
inline unsigned int Point<S, T>::to_int_not_null() const
{
    LIBBOARDGAME_ASSERT(! is_null());
    return m_i;
}

template<unsigned int S, class T>
inline string Point<S, T>::to_string() const
{
    ostringstream s;
    write(s);
    return s.str();
}

template<unsigned int S, class T>
void Point<S, T>::write(ostream& out) const
{
    if (is_onboard())
        StringRep::write(out, get_x(), get_y());
    else if (*this == Point::null())
        out << "NULL";
    else
        out << "OFFBOARD(" << m_i << ')';
}

//-----------------------------------------------------------------------------

template<unsigned int S, class T>
inline ostream& operator<<(ostream& out, const Point<S, T>& p)
{
    p.write(out);
    return out;
}

/** Read point from input stream.
    The function guarantees to support the point representation used by
    @ref libboardgame_doc_gtp, so it can be used for parsing GTP arguments. */
template<unsigned int S, class T>
inline istream& operator>>(istream& in, Point<S, T>& p)
{
    Point<S, T>::read(in, p);
    return in;
}

//-----------------------------------------------------------------------------

} // namespace boardgame_libboardgame

//-----------------------------------------------------------------------------

namespace std {

template<unsigned int S, class T>
struct hash<libboardgame_base::Point<S, T>>
    : public unary_function<libboardgame_base::Point<S, T>, size_t>
{
    size_t operator()(const libboardgame_base::Point<S, T>& p) const
    {
        return p.to_int();
    }
};

} // namespace std

//-----------------------------------------------------------------------------

namespace libboardgame {
namespace util {

template<unsigned int S, class T>
inline bool is_null(const libboardgame_base::Point<S, T>& p)
{
    return p.is_null();
}

template<unsigned int S, class T>
inline void set_null(libboardgame_base::Point<S, T>& p)
{
    p = libboardgame_base::Point<S, T>::null();
}

} // namespace util
} // namespace libboardgame

//-----------------------------------------------------------------------------

/** Macro to unroll a loop over the four adjacent points. */
#define LIBBOARDGAME_FOREACH_ADJ(p, p_adj, statement)       \
    {                                                   \
        { auto p_adj = (p).get_down();  statement; }   \
        { auto p_adj = (p).get_left();  statement; }   \
        { auto p_adj = (p).get_right(); statement; }   \
        { auto p_adj = (p).get_up();    statement; }   \
    }

/** Macro to unroll a loop over the four adjacent points with counter.
    This can be used to store intermediate values in an array indexed by
    a counter to reduce data dependencies between the loop iterations and
    improve optimization possibilities for CPU and compiler. */
#define LIBBOARDGAME_FOREACH_ADJ_CNT(p, p_adj, cnt, statement)           \
    {                                                                \
        { int cnt = 0; auto p_adj = (p).get_down();  statement; }   \
        { int cnt = 1; auto p_adj = (p).get_left();  statement; }   \
        { int cnt = 2; auto p_adj = (p).get_right(); statement; }   \
        { int cnt = 3; auto p_adj = (p).get_up();    statement; }   \
    }

/** Macro to unroll a loop over the eight adjacent and diagonal points. */
#define LIBBOARDGAME_FOREACH_ADJ_DIAG(p, p_adj, statement)       \
    {                                                        \
        { auto p_adj = (p).get_down();       statement; }   \
        { auto p_adj = (p).get_left();       statement; }   \
        { auto p_adj = (p).get_right();      statement; }   \
        { auto p_adj = (p).get_up();         statement; }   \
        { auto p_adj = (p).get_down_left();  statement; }   \
        { auto p_adj = (p).get_down_right(); statement; }   \
        { auto p_adj = (p).get_up_left();    statement; }   \
        { auto p_adj = (p).get_up_right();   statement; }   \
    }

/** Macro to unroll a loop over the four diagonal points. */
#define LIBBOARDGAME_FOREACH_DIAG(p, p_adj, statement)           \
    {                                                        \
        { auto p_adj = (p).get_down_left();  statement; }   \
        { auto p_adj = (p).get_down_right(); statement; }   \
        { auto p_adj = (p).get_up_left();    statement; }   \
        { auto p_adj = (p).get_up_right();   statement; }   \
    }

//-----------------------------------------------------------------------------

#endif // LIBBOARDGAME_BASE_POINT_H
