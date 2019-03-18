//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceMap.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_PIECE_MAP_H
#define LIBPENTOBI_BASE_PIECE_MAP_H

#include <array>
#include <algorithm>
#include "Piece.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** Container mapping a unique piece to another element type.
    The elements must be default-constructible. */
template<typename T>
class PieceMap
{
public:
    PieceMap() = default;

    explicit PieceMap(const T& val) { fill(val); }

    bool operator==(const PieceMap& piece_map) const;

    T& operator[](Piece piece);

    const T& operator[](Piece piece) const;

    void fill(const T& val) { m_a.fill(val); }

private:
    std::array<T, Piece::range_not_null> m_a;
};

template<typename T>
bool PieceMap<T>::operator==(const PieceMap& piece_map) const
{
    return equal(m_a.begin(), m_a.end(), piece_map.m_a.begin());
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

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PIECE_MAP_H
