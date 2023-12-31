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

    bool operator==(const PieceMap& piece_map) const {
        return std::equal(m_a.begin(), m_a.end(), piece_map.m_a.begin()); }

    T& operator[](Piece piece) { return m_a[piece.to_int()]; }

    const T& operator[](Piece piece) const { return m_a[piece.to_int()]; }

    void fill(const T& val) { m_a.fill(val); }

private:
    std::array<T, Piece::max_pieces> m_a;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PIECE_MAP_H
