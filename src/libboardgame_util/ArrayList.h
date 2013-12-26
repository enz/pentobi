//-----------------------------------------------------------------------------
/** @file libboardgame_util/ArrayList.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_ARRAY_LIST_H
#define LIBBOARDGAME_UTIL_ARRAY_LIST_H

#include <algorithm>
#include <cstddef>
#include <iosfwd>
#include <limits>
#include "Assert.h"

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

/** Array-based list with maximum number of elements.
    The user is responsible for not inserting more than the maximum number of
    elements. The elements must be default-constructible. If the size of the
    list shrinks, the destructor of elements will be not be called immediately.
    The list contains iterator definitions that are compatible with STL
    containers.
    @tparam T The type of the elements
    @tparam M The maximum number of elements
    @tparam I The integer type for the array size */
template<typename T, unsigned M, typename I = unsigned>
class ArrayList
{
public:
    static_assert(numeric_limits<I>::is_integer, "");

    typedef T* iterator;

    typedef const T* const_iterator;

    typedef T value_type;

    static const I max_size = M;

    ArrayList();

    /** Construct list with a single element. */
    explicit ArrayList(const T& t);

    T& operator[](I i);

    const T& operator[](I i) const;

    bool operator==(const ArrayList& array_list) const;

    bool operator!=(const ArrayList& array_list) const;

    iterator begin();

    const_iterator begin() const;

    iterator end();

    const_iterator end() const;

    T& back();

    const T& back() const;

    I size() const;

    bool empty() const;

    const T& pop_back();

    void push_back(const T& t);

    void clear();

    void assign(const T& t);

    /** Change the size of the list.
        Does not call constructors on new elements if the size grows or
        destructors of elements if the size shrinks. */
    void resize(I size);

    bool contains(const T& t) const;

    /** Push back element if not already contained in list.
        @return @c true if element was not already in list. */
    bool include(const T& t);

    /** Removal of first occurance of value.
        Preserves the order of elements.
        @return @c true if value was removed. */
    bool remove(const T& t);

    /** Fast removal of element.
        Does not preserve the order of elements. The element will be replaced
        with the last element and the list size decremented. */
    void remove_fast(iterator i);

    /** Fast removal of first occurance of value.
        Does not preserve the order of elements. If the value is found,
        it will be replaced with the last element and the list size
        decremented.
        @return @c true if value was removed. */
    bool remove_fast(const T& t);

    bool is_permutation(const ArrayList& l) const;

private:
    I m_size;

    T m_a[max_size];
};

template<typename T, unsigned M, typename I>
inline ArrayList<T,M,I>::ArrayList()
    : m_size(0)
{
}

template<typename T, unsigned M, typename I>
inline ArrayList<T,M,I>::ArrayList(const T& t)
{
    assign(t);
}

template<typename T, unsigned M, typename I>
inline T& ArrayList<T,M,I>::operator[](I i)
{
    LIBBOARDGAME_ASSERT(i < m_size);
    return m_a[i];
}

template<typename T, unsigned M, typename I>
inline const T& ArrayList<T,M,I>::operator[](I i) const
{
    LIBBOARDGAME_ASSERT(i < m_size);
    return m_a[i];
}

template<typename T, unsigned M, typename I>
bool ArrayList<T,M,I>::operator==(const ArrayList& array_list) const
{
    if (m_size != array_list.m_size)
        return false;
    const T* elem_this = m_a;
    const T* elem_other = array_list.m_a;
    for (I i = 0; i < m_size; ++i, ++elem_this, ++elem_other)
        if (*elem_this != *elem_other)
            return false;
    return true;
}

template<typename T, unsigned M, typename I>
bool ArrayList<T,M,I>::operator!=(const ArrayList& array_list) const
{
    return ! operator==(array_list);
}

template<typename T, unsigned M, typename I>
inline void ArrayList<T,M,I>::assign(const T& t)
{
    m_size = 1;
    m_a[0] = t;
}

template<typename T, unsigned M, typename I>
inline T& ArrayList<T,M,I>::back()
{
    LIBBOARDGAME_ASSERT(m_size > 0);
    return m_a[m_size - 1];
}

template<typename T, unsigned M, typename I>
inline const T& ArrayList<T,M,I>::back() const
{
    LIBBOARDGAME_ASSERT(m_size > 0);
    return m_a[m_size - 1];
}

template<typename T, unsigned M, typename I>
inline auto ArrayList<T,M,I>::begin() -> iterator
{
    return m_a;
}

template<typename T, unsigned M, typename I>
inline auto ArrayList<T,M,I>::begin() const -> const_iterator
{
    return m_a;
}

template<typename T, unsigned M, typename I>
inline void ArrayList<T,M,I>::clear()
{
    m_size = 0;
}

template<typename T, unsigned M, typename I>
bool ArrayList<T,M,I>::contains(const T& t) const
{
    for (const_iterator i = begin(); i != end(); ++i)
        if (*i == t)
            return true;
    return false;
}

template<typename T, unsigned M, typename I>
inline bool ArrayList<T,M,I>::empty() const
{
    return m_size == 0;
}

template<typename T, unsigned M, typename I>
inline auto ArrayList<T,M,I>::end() -> iterator
{
    return begin() + m_size;
}

template<typename T, unsigned M, typename I>
inline auto ArrayList<T,M,I>::end() const -> const_iterator
{
    return begin() + m_size;
}

template<typename T, unsigned M, typename I>
bool ArrayList<T,M,I>::include(const T& t)
{
    iterator i;
    for (i = begin(); i != end(); ++i)
        if (*i == t)
            return false;
    LIBBOARDGAME_ASSERT(m_size < max_size);
    *i = t;
    ++m_size;
    return true;
}

template<typename T, unsigned M, typename I>
bool ArrayList<T,M,I>::is_permutation(const ArrayList& l) const
{
    if (size() != l.size())
        return false;
    ArrayList sorted_this(*this);
    sort(sorted_this.begin(), sorted_this.end());
    ArrayList sorted_other(l);
    sort(sorted_other.begin(), sorted_other.end());
    return equal(sorted_this.begin(), sorted_this.end(), sorted_other.begin());
}

template<typename T, unsigned M, typename I>
inline const T& ArrayList<T,M,I>::pop_back()
{
    LIBBOARDGAME_ASSERT(m_size > 0);
    return m_a[--m_size];
}

template<typename T, unsigned M, typename I>
inline void ArrayList<T,M,I>::push_back(const T& t)
{
    LIBBOARDGAME_ASSERT(m_size < max_size);
    m_a[m_size++] = t;
}

template<typename T, unsigned M, typename I>
inline bool ArrayList<T,M,I>::remove(const T& t)
{
    T* end = this->end();
    for (T* i = m_a; i != end; ++i)
        if (*i == t)
        {
            --end;
            for ( ; i != end; ++i)
                *i = *(i + 1);
            --m_size;
            return true;
        }
    return false;
}

template<typename T, unsigned M, typename I>
inline bool ArrayList<T,M,I>::remove_fast(const T& t)
{
    T* end = this->end();
    for (T* i = m_a; i != end; ++i)
        if (*i == t)
        {
            remove_fast(i);
            return true;
        }
    return false;
}

template<typename T, unsigned M, typename I>
inline void ArrayList<T,M,I>::remove_fast(iterator i)
{
    LIBBOARDGAME_ASSERT(i >= begin());
    LIBBOARDGAME_ASSERT(i < end());
    --m_size;
    *i = *(m_a + m_size);
}

template<typename T, unsigned M, typename I>
inline void ArrayList<T,M,I>::resize(I size)
{
    LIBBOARDGAME_ASSERT(size <= max_size);
    m_size = size;
}

template<typename T, unsigned M, typename I>
inline I ArrayList<T,M,I>::size() const
{
    return m_size;
}

//-----------------------------------------------------------------------------

template<typename T, unsigned M, typename I>
ostream& operator<<(ostream& out, const ArrayList<T,M,I>& l)
{
    auto begin = l.begin();
    auto end = l.end();
    if (begin != end)
    {
        out << *begin;
        for (auto i = begin + 1; i != end; ++i)
            out << ' ' << *i;
    }
    return out;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_ARRAY_LIST_H
