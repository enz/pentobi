//-----------------------------------------------------------------------------
/** @file libboardgame_util/BitMarker.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_BIT_MARKER_H
#define LIBBOARDGAME_UTIL_BIT_MARKER_H

#include <cstring>
#include <limits>

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

/** Bitset to mark objects that are integer wrappers.
    Similar to std::bitset but the objects are used directly as indexes (for
    better type safety and to avoid explicit conversions). It also contains
    additional functionality not present is std::bitset (like
    clear_all_set_known()).
    @tparam T The object class. Requires a function T::to_int() and a constant
    T::range */
template<class T>
class BitMarker
{
public:
    BitMarker();

    /** Clear all (slow). */
    void clear();

    void clear(T t);

    template<class CONTAINER>
    void clear(const CONTAINER& container);

    /** Clear all from a list optimized for the case when we know that the
        list contains all marked objects in the marker.
        This function assumes that all objects not in the list are not marked
        and is faster than clear(CONTAINER). */
    template<class CONTAINER>
    void clear_all_set_known(const CONTAINER& container);

    /** Clear the word that contains an object.
        For use cases when the side effect that other objects are also cleared
        can be ignored (e.g. the implementation of clear_all_set_known()) */
    void clear_word(T t);

    void set(T t);

    /** Mark all (slow). */
    void set_all();

    bool operator[](T t) const;

private:
    typedef unsigned Word;

    typedef unsigned IntType;

    static const IntType word_bits = numeric_limits<Word>::digits;

    static const IntType array_size =
        T::range / word_bits + (T::range % word_bits == 0 ? 0 : 1);

    static IntType get_index(T t);

    static Word get_mask(T t);

    Word m_array[array_size];
};

template<class T>
inline BitMarker<T>::BitMarker()
{
    clear();
}

template<class T>
inline bool BitMarker<T>::operator[](T t) const
{
    return ((m_array[get_index(t)] & get_mask(t)) != 0);
}

template<class T>
inline void BitMarker<T>::clear()
{
    memset(m_array, 0, array_size * sizeof(Word));
}

template<class T>
inline void BitMarker<T>::clear(T t)
{
    m_array[get_index(t)] &= ~get_mask(t);
}

template<class T>
template<class CONTAINER>
inline void BitMarker<T>::clear(const CONTAINER& container)
{
    for (T t : container)
        clear(t);
}

template<class T>
template<class CONTAINER>
inline void BitMarker<T>::clear_all_set_known(const CONTAINER& container)
{
    for (T t : container)
        clear_word(t);
}

template<class T>
inline void BitMarker<T>::clear_word(T t)
{
    m_array[get_index(t)] = 0;
}

template<class T>
inline auto BitMarker<T>::get_index(T t) -> IntType
{
    return t.to_int() / word_bits;
}

template<class T>
inline auto BitMarker<T>::get_mask(T t) -> Word
{
    return static_cast<Word>(1) << (t.to_int() % word_bits);
}

template<class T>
inline void BitMarker<T>::set(T t)
{
    m_array[get_index(t)] |= get_mask(t);
}

template<class T>
inline void BitMarker<T>::set_all()
{
    memset(m_array, ~0, array_size * sizeof(Word));
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_BIT_MARKER_H
