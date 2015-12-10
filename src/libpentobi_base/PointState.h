//-----------------------------------------------------------------------------
/** @file libpentobi_base/PointState.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_POINTSTATE_H
#define LIBPENTOBI_BASE_POINTSTATE_H

#include "Color.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

/** State of an on-board point, which can be a color or empty */
class PointState
{
public:
    typedef Color::IntType IntType;

    class Iterator
    {
        friend class PointState;

    public:
        Iterator();

        explicit operator bool() const;

        void operator++();

        PointState operator*() const;

    private:
        IntType m_i;
    };

    static const IntType range = Color::range + 1;

    static const IntType value_empty = range - 1;

    PointState();

    PointState(Color c);

    explicit PointState(IntType i);

    bool operator==(const PointState& s) const;

    bool operator!=(const PointState& s) const;

    bool operator==(const Color& c) const;

    bool operator!=(const Color& c) const;

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

typedef PointState::Iterator PointStateIterator;

inline PointState::Iterator::Iterator()
    : m_i(0)
{
}

inline PointState::Iterator::operator bool() const
{
    return m_i < PointState::range;
}

inline void PointState::Iterator::operator++()
{
    ++m_i;
}

inline PointState PointState::Iterator::operator*() const
{
    LIBBOARDGAME_ASSERT(*this);
    return PointState(m_i);
}

inline PointState::PointState()
{
#if LIBBOARDGAME_DEBUG
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

inline bool PointState::operator==(const PointState& p) const
{
    return m_i == p.m_i;
}

inline bool PointState::operator==(const Color& c) const
{
    return m_i == c.to_int();
}

inline bool PointState::operator!=(const PointState& s) const
{
    return ! operator==(s);
}

inline bool PointState::operator!=(const Color& c) const
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

ostream& operator<<(ostream& out, const PointState& s);

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_POINTSTATE_H
