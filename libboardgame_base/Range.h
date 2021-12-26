//-----------------------------------------------------------------------------
/** @file libboardgame_base/Range.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_RANGE_H
#define LIBBOARDGAME_BASE_RANGE_H

#include <cstddef>

namespace libboardgame_base {

//-----------------------------------------------------------------------------

template<typename T>
class Range
{
public:
    Range() = default;

    Range(T* begin, T* end)
        : m_begin(begin),
          m_end(end)
    { }

    T* begin() const { return m_begin; }

    T* end() const { return m_end; }

    size_t size() const { return m_end - m_begin; }

    bool empty() const { return m_begin == m_end; }

    bool contains(T& t) const;

private:
    T* m_begin;

    T* m_end;
};

template<typename T>
bool Range<T>::contains(T& t) const
{
    for (auto& i : *this)
        if (i == t)
            return true;
    return false;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_RANGE_H
