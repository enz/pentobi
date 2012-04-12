//-----------------------------------------------------------------------------
/** @file libpentobi_base/Piece.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_PIECE_H
#define LIBPENTOBI_BASE_PIECE_H

#include "libboardgame_util/Assert.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Wrapper around an integer representing a piece type in a certain
    game variant. */
class Piece
{
public:
    /** Maximum number of unique pieces per color. */
    static const unsigned int max_pieces = 22;

    /** Integer range used for unique pieces without the null piece. */
    static const unsigned int range_not_null = max_pieces;

    /** Integer range used for unique pieces including the null piece */
    static const unsigned int range = max_pieces + 1;

    static Piece null();

    Piece();

    explicit Piece(unsigned int i);

    Piece& operator=(const Piece& piece);

    bool operator==(const Piece& piece) const;

    bool operator!=(const Piece& piece) const;

    bool is_null() const;

    /** Return move as an integer between 0 and Piece::range */
    unsigned int to_int() const;

private:
    static const unsigned short value_null = range - 1;

    static const unsigned short value_uninitialized = range;

    unsigned int m_i;

    bool is_initialized() const;
};

inline Piece::Piece()
{
#if LIBBOARDGAME_DEBUG
    m_i = value_uninitialized;
#endif
}

inline Piece::Piece(unsigned int i)
{
    LIBBOARDGAME_ASSERT(i < range);
    m_i = static_cast<unsigned int>(i);
}

inline Piece& Piece::operator=(const Piece& piece)
{
    m_i = piece.m_i;
    return *this;
}

inline bool Piece::operator==(const Piece& piece) const
{
    return m_i == piece.m_i;
}

inline bool Piece::operator!=(const Piece& piece) const
{
    return ! operator==(piece);
}

inline bool Piece::is_initialized() const
{
    return m_i < value_uninitialized;
}

inline bool Piece::is_null() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i == value_null;
}

inline Piece Piece::null()
{
    return Piece(value_null);
}

inline unsigned int Piece::to_int() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_BASE_PIECE_H
