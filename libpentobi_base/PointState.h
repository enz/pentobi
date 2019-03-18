//-----------------------------------------------------------------------------
/** @file libpentobi_base/PointState.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_POINT_STATE_H
#define LIBPENTOBI_BASE_POINT_STATE_H

#include "Color.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** State of an on-board point, which can be a color or empty */
class PointState
{
public:
    using IntType = Color::IntType;

    static constexpr IntType range = Color::range + 1;

    static constexpr IntType value_empty = range - 1;


    static PointState empty() { return PointState(value_empty); }


    PointState();

    explicit PointState(Color c) { m_i = c.to_int(); }

    explicit PointState(IntType i);

    bool operator==(PointState s) const { return m_i == s.m_i; }

    bool operator!=(PointState s) const { return ! operator==(s); }

    bool operator==(Color c) const { return m_i == c.to_int(); }

    bool operator!=(Color c) const { return ! operator==(c); }

    IntType to_int() const;

    bool is_empty() const;

    bool is_color() const;

    Color to_color() const;

private:
    static constexpr IntType value_uninitialized = range;

    IntType m_i;


#ifdef LIBBOARDGAME_DEBUG
    bool is_initialized() const { return m_i < value_uninitialized; }
#endif
};


inline PointState::PointState()
{
#ifdef LIBBOARDGAME_DEBUG
    m_i = value_uninitialized;
#endif
}

inline PointState::PointState(IntType i)
{
    LIBBOARDGAME_ASSERT(i < range);
    m_i = i;
}

inline bool PointState::is_color() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i != value_empty;
}

inline bool PointState::is_empty() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i == value_empty;
}

inline Color PointState::to_color() const
{
    LIBBOARDGAME_ASSERT(is_color());
    return Color(m_i);
}

inline PointState::IntType PointState::to_int() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_POINT_STATE_H
