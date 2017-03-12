//-----------------------------------------------------------------------------
/** @file libboardgame_base/Point.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_POINT_H
#define LIBBOARDGAME_BASE_POINT_H

#include <limits>
#include "libboardgame_util/Assert.h"
#include "libboardgame_sys/Compiler.h"

namespace libboardgame_base {

using namespace std;
using namespace libboardgame_util;

//-----------------------------------------------------------------------------

/** Coordinate on the board.
    Depending on the game, a point represents a field or intersection (in Go)
    on the board. The class is a lightweight wrapper around an integer. All
    information about points including the coordinates are contained in
    Geometry. The convention for the coordinates is that the top left corner of
    the board has the coordinates (0,0). Point::null() has the meaning
    "no point".
    @tparam M The maximum number of on-board points of all geometries this
    point is used in (excluding the null point).
    @tparam I An unsigned integer type to store the point value. */
template<unsigned M, typename I>
class Point
{
public:
    typedef I IntType;

    static const unsigned max_onboard = M;

    static_assert(numeric_limits<I>::is_integer, "");

    static_assert(! numeric_limits<I>::is_signed, "");

    static const unsigned range_onboard = max_onboard;

    static const unsigned range = max_onboard + 1;

    static Point null();

    LIBBOARDGAME_FORCE_INLINE Point();

    explicit Point(unsigned i);

    bool operator==(const Point& p) const;

    bool operator!=(const Point& p) const;

    bool operator<(const Point& p) const;

    bool is_null() const;

    /** Return point as an integer between 0 and Point::range */
    unsigned to_int() const;

private:
    static const IntType value_uninitialized = range;

    static const IntType value_null = range - 1;

    IntType m_i;

    LIBBOARDGAME_FORCE_INLINE bool is_initialized() const;
};

template<unsigned M, typename I>
inline Point<M, I>::Point()
{
#if LIBBOARDGAME_DEBUG
    m_i = value_uninitialized;
#endif
}

template<unsigned M, typename I>
inline Point<M, I>::Point(unsigned i)
{
    LIBBOARDGAME_ASSERT(i < range);
    m_i = static_cast<I>(i);
}

template<unsigned M, typename I>
inline bool Point<M, I>::operator==(const Point& p) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(p.is_initialized());
    return m_i == p.m_i;
}

template<unsigned M, typename I>
inline bool Point<M, I>::operator!=(const Point& p) const
{
    return ! operator==(p);
}

template<unsigned M, typename I>
inline bool Point<M, I>::operator<(const Point& p) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(p.is_initialized());
    return m_i < p.m_i;
}

template<unsigned M, typename I>
inline bool Point<M, I>::is_initialized() const
{
    return m_i < value_uninitialized;
}

template<unsigned M, typename I>
inline bool Point<M, I>::is_null() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i == value_null;
}

template<unsigned M, typename I>
inline auto Point<M, I>::null() -> Point
{
    return Point(value_null);
}

template<unsigned M, typename I>
inline unsigned Point<M, I>::to_int() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_POINT_H
