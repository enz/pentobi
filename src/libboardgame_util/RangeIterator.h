//-----------------------------------------------------------------------------
/** @file RangeIterator.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_RANGE_ITERATOR_H
#define LIBBOARDGAME_UTIL_RANGE_ITERATOR_H

#include "Assert.h"

namespace libboardgame_util {

//-----------------------------------------------------------------------------

template<typename T>
class RangeConstIterator
{
public:
    typedef T Type;

    RangeConstIterator(const Type* begin, const Type* end);

    operator bool() const;

    void operator++();

    const Type& operator*() const;

private:
    const Type* m_current;

    const Type* m_end;
};

template<typename T>
RangeConstIterator<T>::RangeConstIterator(const Type* begin, const Type* end)
    : m_current(begin),
      m_end(end)
{
    LIBBOARDGAME_ASSERT(end >= begin);
}

template<typename T>
RangeConstIterator<T>::operator bool() const
{
    return m_current != m_end;
}

template<typename T>
void RangeConstIterator<T>::operator++()
{
    LIBBOARDGAME_ASSERT(operator bool());
    ++m_current;
}

template<typename T>
const typename RangeConstIterator<T>::Type& RangeConstIterator<T>::operator*()
                                                                          const
{
    LIBBOARDGAME_ASSERT(operator bool());
    return *m_current;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_RANGE_ITERATOR_H
