//-----------------------------------------------------------------------------
/** @file libpentobi_base/Color.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_COLOR_H
#define LIBPENTOBI_BASE_COLOR_H

#include <cstdint>
#include <iosfwd>
#include <string>
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/Exception.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_util::Exception;

//-----------------------------------------------------------------------------

class Color
{
public:
    typedef uint_fast8_t IntType;

    class InvalidString
        : public Exception
    {
    public:
        InvalidString(const string& s);
    };

    class Iterator
    {
        friend class Color;

    public:
        Iterator(IntType nu_colors);

        operator bool() const;

        void operator++();

        Color operator*() const;

    private:
        const IntType m_nu_colors;

        IntType m_i;
    };

    static const IntType range = 4;

    Color();

    Color(const Color& c);

    explicit Color(IntType i);

    explicit Color(const string& s);

    bool operator==(const Color& c) const;

    bool operator!=(const Color& c) const;

    bool operator<(const Color& c) const;

    IntType to_int() const;

    Color get_next(IntType nu_colors) const;

    Color get_previous(IntType nu_colors) const;

private:
    static const IntType value_uninitialized = range;

    IntType m_i;

    bool is_initialized() const;
};

typedef Color::Iterator ColorIterator;

inline Color::InvalidString::InvalidString(const string& s)
    : Exception(s)
{
}

inline Color::Iterator::Iterator(IntType nu_colors)
    : m_nu_colors(nu_colors),
      m_i(0)
{
}

inline Color::Iterator::operator bool() const
{
    return m_i < m_nu_colors;
}

inline void Color::Iterator::operator++()
{
    ++m_i;
}

inline Color Color::Iterator::operator*() const
{
    LIBBOARDGAME_ASSERT(operator bool());
    return Color(m_i);
}

inline Color::Color()
{
#if LIBBOARDGAME_DEBUG
    m_i = value_uninitialized;
#endif
}

inline Color::Color(IntType i)
{
    LIBBOARDGAME_ASSERT(i < range);
    m_i = i;
}

inline Color::Color(const Color& c)
{
    m_i = c.m_i;
}

inline bool Color::operator==(const Color& c) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(c.is_initialized());
    return (m_i == c.m_i);
}

inline bool Color::operator!=(const Color& c) const
{
    return ! operator==(c);
}

inline bool Color::operator<(const Color& c) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(c.is_initialized());
    return (m_i < c.m_i);
}

inline Color Color::get_next(IntType nu_colors) const
{
    IntType i = static_cast<IntType>(m_i + 1);
    if (i == nu_colors)
        return Color(0);
    return Color(i);
}

inline Color Color::get_previous(IntType nu_colors) const
{
    LIBBOARDGAME_ASSERT(nu_colors > 1);
    if (m_i == 0)
        return Color(static_cast<IntType>(nu_colors - 1));
    return Color(static_cast<IntType>(m_i - 1));
}

inline bool Color::is_initialized() const
{
    return m_i < value_uninitialized;
}

inline Color::IntType Color::to_int() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i;
}

//-----------------------------------------------------------------------------

ostream& operator<<(ostream& out, const Color& c);

/** Read color from input stream.
    The function guaratees to support all string representations allowed by
    @ref libboardgame_doc_gtp, so it can be used for parsing GTP arguments. */
istream& operator>>(istream& in, Color& c);

//-----------------------------------------------------------------------------

/** Macro to unroll a loop over all colors. */
#define LIBPENTOBI_FOREACH_COLOR(c, statement)               \
    {                                                        \
        static_assert(Color::range == 4, "");                \
        { Color c(0);  statement; }                          \
        { Color c(1);  statement; }                          \
        { Color c(2);  statement; }                          \
        { Color c(3);  statement; }                          \
    }

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_COLOR_H
