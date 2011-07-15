//-----------------------------------------------------------------------------
/** @file libpentobi_base/PointStateExt.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_POINTSTATEEXT_H
#define LIBPENTOBI_BASE_POINTSTATEEXT_H

#include "PointState.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

/** State of an on-board or off-board point for games in which the point state
    can be a color, empty, or off-board */
class PointStateExt
{
public:
    class Iterator
    {
        friend class PointStateExt;

    public:
        Iterator();

        operator bool() const;

        void operator++();

        PointStateExt operator*() const;

    private:
        unsigned int m_i;
    };

    static const unsigned int range = PointState::range + 1;

    PointStateExt();

    PointStateExt(PointState s);

    PointStateExt(Color c);

    PointStateExt(const PointStateExt& s);

    explicit PointStateExt(unsigned int i);

    bool operator==(const PointStateExt& s) const;

    bool operator!=(const PointStateExt& s) const;

    bool operator==(const PointState& s) const;

    bool operator!=(const PointState& s) const;

    bool operator==(const Color& c) const;

    bool operator!=(const Color& c) const;

    unsigned int to_int() const;

    static PointStateExt offboard();

    bool is_offboard() const;

    bool is_empty() const;

    PointState to_point_state() const;

    Color to_color() const;

    bool is_color() const;

private:
    static const unsigned int value_offboard = range - 1;

    static const unsigned int value_uninitialized = range;

    unsigned int m_i;

    bool is_initialized() const;
};

typedef PointStateExt::Iterator PointStateExtIterator;

inline PointStateExt::Iterator::Iterator()
    : m_i(0)
{
}

inline PointStateExt::Iterator::operator bool() const
{
    return m_i < PointStateExt::range;
}

inline void PointStateExt::Iterator::operator++()
{
    ++m_i;
}

inline PointStateExt PointStateExt::Iterator::operator*() const
{
    LIBBOARDGAME_ASSERT(operator bool());
    return PointStateExt(m_i);
}

inline PointStateExt::PointStateExt()
{
#if LIBBOARDGAME_DEBUG
    m_i = value_uninitialized;
#endif
}

inline PointStateExt::PointStateExt(PointState s)
{
    m_i = s.to_int();
}

inline PointStateExt::PointStateExt(Color c)
{
    m_i = c.to_int();
}

inline PointStateExt::PointStateExt(unsigned int i)
{
    LIBBOARDGAME_ASSERT(i < range);
    m_i = i;
}

inline PointStateExt::PointStateExt(const PointStateExt& s)
{
    m_i = s.m_i;
}

inline bool PointStateExt::operator==(const PointStateExt& s) const
{
    return m_i == s.m_i;
}

inline bool PointStateExt::operator==(const PointState& s) const
{
    return m_i == s.to_int();
}

inline bool PointStateExt::operator==(const Color& c) const
{
    return m_i == c.to_int();
}

inline bool PointStateExt::operator!=(const PointStateExt& s) const
{
    return ! operator==(s);
}

inline bool PointStateExt::operator!=(const PointState& s) const
{
    return ! operator==(s);
}

inline bool PointStateExt::operator!=(const Color& c) const
{
    return ! operator==(c);
}

inline bool PointStateExt::is_offboard() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i == value_offboard;
}

inline bool PointStateExt::is_color() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i < Color::range;
}

inline bool PointStateExt::is_empty() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i == PointState::value_empty;
}

inline bool PointStateExt::is_initialized() const
{
    return m_i < value_uninitialized;
}

inline PointStateExt PointStateExt::offboard()
{
    return PointStateExt(value_offboard);
}

inline Color PointStateExt::to_color() const
{
    return to_point_state().to_color();
}

inline PointState PointStateExt::to_point_state() const
{
    LIBBOARDGAME_ASSERT(! is_offboard());
    return PointState(m_i);
}

inline unsigned int PointStateExt::to_int() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i;
}

//-----------------------------------------------------------------------------

ostream& operator<<(ostream& out, const PointStateExt& s);

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_POINTSTATEEXT_H
