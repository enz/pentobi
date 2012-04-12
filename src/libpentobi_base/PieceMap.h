//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceMap.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_PIECE_MAP_H
#define LIBPENTOBI_BASE_PIECE_MAP_H

#include "Piece.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** Container mapping a unique piece to another element type.
    The elements must be default-constructible. */
template<typename T>
class PieceMap
{
public:
    PieceMap();

    PieceMap(const T& val);

    T& operator[](Piece piece);

    const T& operator[](Piece piece) const;

    void fill(const T& val);

private:
    T m_a[Piece::range_not_null];
};

template<typename T>
inline PieceMap<T>::PieceMap()
{
}

template<typename T>
inline PieceMap<T>::PieceMap(const T& val)
{
    fill(val);
}

template<typename T>
inline T& PieceMap<T>::operator[](Piece piece)
{
    LIBBOARDGAME_ASSERT(! piece.is_null());
    return m_a[piece.to_int()];
}

template<typename T>
inline const T& PieceMap<T>::operator[](Piece piece) const
{
    LIBBOARDGAME_ASSERT(! piece.is_null());
    return m_a[piece.to_int()];
}

template<typename T>
void PieceMap<T>::fill(const T& val)
{
    for (unsigned int i = 0; i < Piece::range_not_null; ++i)
        m_a[i] = val;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PIECE_MAP_H
