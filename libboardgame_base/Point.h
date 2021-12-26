//-----------------------------------------------------------------------------
/** @file libboardgame_base/Point.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_POINT_H
#define LIBBOARDGAME_BASE_POINT_H

#include <limits>
#include "Assert.h"
#include "Compiler.h"

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Coordinate on the board.
    Depending on the game, a point represents a field or intersection (in Go)
    on the board. The class is a lightweight wrapper around an integer. All
    information about points including the coordinates are contained in
    Geometry. The convention for the coordinates is that the top left corner of
    the board has the coordinates (0,0). Point::null() has the meaning
    "no point".
    @tparam M The maximum number of on-board points of all geometries this
    point is used in (excluding the null point). This may be smaller than
    W*H if the geomtries are not rectangular.
    @tparam W The maximum width of all geometries this point is used in.
    @tparam H The maximum height of all geometries this point is used in.
    @tparam I An unsigned integer type to store the point value. */
template<unsigned M, unsigned W, unsigned H, typename I>
class Point
{
public:
    using IntType = I;

    static constexpr unsigned range_onboard = M;

    static constexpr unsigned max_width = W;

    static constexpr unsigned max_height = W;

    static_assert(numeric_limits<I>::is_integer);
    static_assert(! numeric_limits<I>::is_signed);
    static_assert(range_onboard <= max_width * max_height);

    static constexpr unsigned range = range_onboard + 1;


    static Point null() { return Point(value_null); }


    LIBBOARDGAME_FORCE_INLINE Point();

    explicit Point(unsigned i);

    bool operator==(const Point& p) const;

    bool operator!=(const Point& p) const;

    bool operator<(const Point& p) const;

    bool is_null() const;

    /** Return point as an integer between 0 and Point::range */
    IntType to_int() const;

private:
    static constexpr IntType value_uninitialized = range;

    static constexpr IntType value_null = range - 1;


    IntType m_i;


#ifdef LIBBOARDGAME_DEBUG
    LIBBOARDGAME_FORCE_INLINE
    bool is_initialized() const { return m_i < value_uninitialized; }
#endif
};


template<unsigned M, unsigned W, unsigned H, typename I>
inline Point<M, W, H, I>::Point()
{
#ifdef LIBBOARDGAME_DEBUG
    m_i = value_uninitialized;
#endif
}

template<unsigned M, unsigned W, unsigned H, typename I>
inline Point<M, W, H, I>::Point(unsigned i)
{
    LIBBOARDGAME_ASSERT(i < range);
    m_i = static_cast<I>(i);
}

template<unsigned M, unsigned W, unsigned H, typename I>
inline bool Point<M, W, H, I>::operator==(const Point& p) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(p.is_initialized());
    return m_i == p.m_i;
}

template<unsigned M, unsigned W, unsigned H, typename I>
inline bool Point<M, W, H, I>::operator!=(const Point& p) const
{
    return ! operator==(p);
}

template<unsigned M, unsigned W, unsigned H, typename I>
inline bool Point<M, W, H, I>::operator<(const Point& p) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(p.is_initialized());
    return m_i < p.m_i;
}

template<unsigned M, unsigned W, unsigned H, typename I>
inline bool Point<M, W, H, I>::is_null() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i == value_null;
}

template<unsigned M, unsigned W, unsigned H, typename I>
inline auto Point<M, W, H, I>::to_int() const -> IntType
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_POINT_H
