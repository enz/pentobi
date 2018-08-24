//-----------------------------------------------------------------------------
/** @file libpentobi_base/PointState.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_POINT_STATE_H
#define LIBPENTOBI_BASE_POINT_STATE_H

#include "Color.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

/** State of an on-board point, which can be a color or empty */
class PointState
{
public:
    using IntType = Color::IntType;

    static const IntType range = Color::range + 1;

    static const IntType value_empty = range - 1;


    PointState();

    explicit PointState(Color c);

    explicit PointState(IntType i);

    bool operator==(PointState s) const;

    bool operator!=(PointState s) const;

    bool operator==(Color c) const;

    bool operator!=(Color c) const;

    IntType to_int() const;

    static PointState empty();

    bool is_empty() const;

    bool is_color() const;

    Color to_color() const;

private:
    static const IntType value_uninitialized = range;

    IntType m_i;

    bool is_initialized() const;
};


inline PointState::PointState()
{
#ifdef LIBBOARDGAME_DEBUG
    m_i = value_uninitialized;
#endif
}

inline PointState::PointState(Color c)
{
    m_i = c.to_int();
}

inline PointState::PointState(IntType i)
{
    LIBBOARDGAME_ASSERT(i < range);
    m_i = i;
}

inline bool PointState::operator==(PointState s) const
{
    return m_i == s.m_i;
}

inline bool PointState::operator==(Color c) const
{
    return m_i == c.to_int();
}

inline bool PointState::operator!=(PointState s) const
{
    return ! operator==(s);
}

inline bool PointState::operator!=(Color c) const
{
    return ! operator==(c);
}

inline PointState PointState::empty()
{
    return PointState(value_empty);
}

inline bool PointState::is_initialized() const
{
    return m_i < value_uninitialized;
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
