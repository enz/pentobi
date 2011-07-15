//-----------------------------------------------------------------------------
/** @file libpentobi_base/PointState.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_POINTSTATE_H
#define LIBPENTOBI_BASE_POINTSTATE_H

#include "Color.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

/** State of an on-board point for games in which the point state can be
    a color or empty */
class PointState
{
public:
    class Iterator
    {
        friend class PointState;

    public:
        Iterator();

        operator bool() const;

        void operator++();

        PointState operator*() const;

    private:
        unsigned int m_i;
    };

    static const unsigned int range = Color::range + 1;

    static const unsigned int value_empty = range - 1;

    PointState();

    PointState(const PointState& s);

    PointState(Color c);

    explicit PointState(unsigned int i);

    bool operator==(const PointState& s) const;

    bool operator!=(const PointState& s) const;

    bool operator==(const Color& c) const;

    bool operator!=(const Color& c) const;

    unsigned int to_int() const;

    static PointState empty();

    bool is_empty() const;

    bool is_color() const;

    Color to_color() const;

private:
    static const unsigned int value_uninitialized = range;

    unsigned int m_i;

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
    LIBBOARDGAME_ASSERT(operator bool());
    return PointState(m_i);
}

inline PointState::PointState()
{
#if LIBBOARDGAME_DEBUG
    m_i = value_uninitialized;
#endif
}

inline PointState::PointState(const PointState& s)
{
    m_i = s.m_i;
}

inline PointState::PointState(Color c)
{
    m_i = c.to_int();
}

inline PointState::PointState(unsigned int i)
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

inline unsigned int PointState::to_int() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i;
}

//-----------------------------------------------------------------------------

ostream& operator<<(ostream& out, const PointState& s);

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_POINTSTATE_H
