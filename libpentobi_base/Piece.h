//-----------------------------------------------------------------------------
/** @file libpentobi_base/Piece.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_PIECE_H
#define LIBPENTOBI_BASE_PIECE_H

#include <cstdint>
#include "libboardgame_base/Assert.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** Wrapper around an integer representing a piece type in a certain
    game variant. */
class Piece
{
public:
    using IntType = std::uint_fast8_t;

    /** Maximum number of unique pieces per color. */
    static constexpr IntType max_pieces = 24;

    /** Integer range used for unique pieces without the null piece. */
    static constexpr IntType range_not_null = max_pieces;

    /** Integer range used for unique pieces including the null piece */
    static constexpr IntType range = max_pieces + 1;


    Piece();

    explicit Piece(IntType i);

    bool operator==(Piece piece) const { return m_i == piece.m_i; }

    bool operator!=(Piece piece) const { return ! operator==(piece); }

    /** Return move as an integer between 0 and Piece::range */
    IntType to_int() const;

private:
    static constexpr IntType value_null = range - 1;

    static constexpr IntType value_uninitialized = range;

    IntType m_i;


#ifdef LIBBOARDGAME_DEBUG
    bool is_initialized() const { return m_i < value_uninitialized; }
#endif
};

inline Piece::Piece()
{
#ifdef LIBBOARDGAME_DEBUG
    m_i = value_uninitialized;
#endif
}

inline Piece::Piece(IntType i)
{
    LIBBOARDGAME_ASSERT(i < range);
    m_i = i;
}

inline auto Piece::to_int() const -> IntType
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_BASE_PIECE_H
