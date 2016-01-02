//----------------------------------------------------------------------------
/** @file libboardgame_util/Range.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_RANGE_H
#define LIBBOARDGAME_UTIL_RANGE_H

namespace libboardgame_util {

//-----------------------------------------------------------------------------

template<typename T>
class Range
{
public:
    Range(T* begin, T* end)
        : m_begin(begin),
          m_end(end)
    { }

    T* begin() const { return m_begin; }

    T* end() const { return m_end; }

private:
    T* m_begin;

    T* m_end;
};

//----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_RANGE_H
