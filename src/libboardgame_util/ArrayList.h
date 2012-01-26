//-----------------------------------------------------------------------------
/** @file ArrayList.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_ARRAY_LIST_H
#define LIBBOARDGAME_UTIL_ARRAY_LIST_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include "Assert.h"

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

/** List with stack-based pre-allocated maximum number of elements.
    The user is responsible for not inserting more than the maximum number of
    elements. The elements must be default-constructible. If the size of the
    list shrinks, the destructor of elements will be not be called immediately.
    The list contains iterator definitions that are compatible with STL
    containers. It uses @c unsigned int, not @c size_t for the number of
    elements to avoid frequent conversions, because LibBoardGame uses @c int for
    the number of moves or other game objects.
    @tparam T The type of the elements
    @tparam M The maximum number of elements */
template<typename T, unsigned int M>
class ArrayList
{
public:
    typedef T* iterator;

    typedef const T* const_iterator;

    typedef T value_type;

    static const unsigned int max_size = M;

    ArrayList();

    /** Construct list with a single element. */
    explicit ArrayList(const T& t);

    T& operator[](unsigned int i);

    const T& operator[](unsigned int i) const;

    bool operator==(const ArrayList& array_list) const;

    bool operator!=(const ArrayList& array_list) const;

    iterator begin();

    const_iterator begin() const;

    iterator end();

    const_iterator end() const;

    T& back();

    const T& back() const;

    unsigned int size() const;

    bool empty() const;

    const T& pop_back();

    void push_back(const T& t);

    void clear();

    void assign(const T& t);

    /** Change the size of the list.
        Does not call constructors on new elements if the size grows or
        destructors of elements if the size shrinks. */
    void resize(unsigned int size);

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
    unsigned int m_size;

    T m_a[max_size];
};

template<typename T, unsigned int M>
inline ArrayList<T, M>::ArrayList()
    : m_size(0)
{
}

template<typename T, unsigned int M>
inline ArrayList<T, M>::ArrayList(const T& t)
{
    assign(t);
}

template<typename T, unsigned int M>
inline T& ArrayList<T, M>::operator[](unsigned int i)
{
    LIBBOARDGAME_ASSERT(i < m_size);
    return m_a[i];
}

template<typename T, unsigned int M>
inline const T& ArrayList<T, M>::operator[](unsigned int i) const
{
    LIBBOARDGAME_ASSERT(i < m_size);
    return m_a[i];
}

template<typename T, unsigned int M>
bool ArrayList<T, M>::operator==(const ArrayList& array_list) const
{
    if (m_size != array_list.m_size)
        return false;
    const T* elem_this = m_a;
    const T* elem_other = array_list.m_a;
    for (unsigned int i = 0; i < m_size; ++i, ++elem_this, ++elem_other)
        if (*elem_this != *elem_other)
            return false;
    return true;
}

template<typename T, unsigned int M>
bool ArrayList<T, M>::operator!=(const ArrayList& array_list) const
{
    return ! operator==(array_list);
}

template<typename T, unsigned int M>
inline void ArrayList<T, M>::assign(const T& t)
{
    m_size = 1;
    m_a[0] = t;
}

template<typename T, unsigned int M>
inline T& ArrayList<T, M>::back()
{
    LIBBOARDGAME_ASSERT(m_size > 0);
    return m_a[m_size - 1];
}

template<typename T, unsigned int M>
inline const T& ArrayList<T, M>::back() const
{
    LIBBOARDGAME_ASSERT(m_size > 0);
    return m_a[m_size - 1];
}

template<typename T, unsigned int M>
inline typename ArrayList<T, M>::iterator ArrayList<T, M>::begin()
{
    return m_a;
}

template<typename T, unsigned int M>
inline typename ArrayList<T, M>::const_iterator ArrayList<T, M>::begin() const
{
    return m_a;
}

template<typename T, unsigned int M>
inline void ArrayList<T, M>::clear()
{
    m_size = 0;
}

template<typename T, unsigned int M>
bool ArrayList<T, M>::contains(const T& t) const
{
    for (const_iterator i = begin(); i != end(); ++i)
        if (*i == t)
            return true;
    return false;
}

template<typename T, unsigned int M>
inline bool ArrayList<T, M>::empty() const
{
    return m_size == 0;
}

template<typename T, unsigned int M>
inline typename ArrayList<T, M>::iterator ArrayList<T, M>::end()
{
    return begin() + m_size;
}

template<typename T, unsigned int M>
inline typename ArrayList<T, M>::const_iterator ArrayList<T, M>::end() const
{
    return begin() + m_size;
}

template<typename T, unsigned int M>
bool ArrayList<T, M>::include(const T& t)
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

template<typename T, unsigned int M>
bool ArrayList<T, M>::is_permutation(const ArrayList& l) const
{
    if (size() != l.size())
        return false;
    ArrayList sorted_this(*this);
    sort(sorted_this.begin(), sorted_this.end());
    ArrayList sorted_other(l);
    sort(sorted_other.begin(), sorted_other.end());
    return equal(sorted_this.begin(), sorted_this.end(), sorted_other.begin());
}

template<typename T, unsigned int M>
inline const T& ArrayList<T, M>::pop_back()
{
    LIBBOARDGAME_ASSERT(m_size > 0);
    return m_a[--m_size];
}

template<typename T, unsigned int M>
inline void ArrayList<T, M>::push_back(const T& t)
{
    LIBBOARDGAME_ASSERT(m_size < max_size);
    m_a[m_size++] = t;
}

template<typename T, unsigned int M>
inline bool ArrayList<T, M>::remove(const T& t)
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

template<typename T, unsigned int M>
inline bool ArrayList<T, M>::remove_fast(const T& t)
{
    T* current = m_a + m_size - 1;
    for (int i = m_size; i--; --current)
        if (*current == t)
        {
            remove_fast(current);
            return true;
        }
    return false;
}

template<typename T, unsigned int M>
inline void ArrayList<T, M>::remove_fast(iterator i)
{
    --m_size;
    *i = *(m_a + m_size);
}

template<typename T, unsigned int M>
inline void ArrayList<T, M>::resize(unsigned int size)
{
    LIBBOARDGAME_ASSERT(size <= max_size);
    m_size = size;
}

template<typename T, unsigned int M>
inline unsigned int ArrayList<T, M>::size() const
{
    return m_size;
}

//-----------------------------------------------------------------------------

template<typename T, unsigned int M>
ostream& operator<<(ostream& out, const ArrayList<T, M>& l)
{
    typedef typename libboardgame_util::ArrayList<T, M>::const_iterator Iterator;
    Iterator begin = l.begin();
    Iterator end = l.end();
    if (begin != end)
    {
        out << *begin;
        for (Iterator i = begin + 1; i != end; ++i)
            out << ' ' << *i;
    }
    return out;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_ARRAY_LIST_H
