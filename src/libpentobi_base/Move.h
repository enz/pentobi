//-----------------------------------------------------------------------------
/** @file libpentobi_base/Move.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MOVE_H
#define LIBPENTOBI_BASE_MOVE_H

#include <climits>
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/Exception.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_util::Exception;

//-----------------------------------------------------------------------------

class Move
{
public:
    static const unsigned int onboard_moves_classic = 30433;

    static const unsigned int onboard_moves_trigon = 32131;

    static const unsigned int onboard_moves_trigon_3 = 24859;

    static const unsigned int onboard_moves_duo = 13729;

    /** Integer range of moves.
        The maximum is given by the number of on-board moves in game variant
        Trigon, plus a pass and a null move. */
    static const unsigned int range = onboard_moves_trigon + 1 + 1;

    static Move pass();

    static Move null();

    Move();

    explicit Move(unsigned int i);

    Move& operator=(const Move& mv);

    bool operator==(const Move& mv) const;

    bool operator!=(const Move& mv) const;

    bool is_pass() const;

    bool is_null() const;

    /** Test if move is a regular move (not a null or pass move) */
    bool is_regular() const;

    /** Return move as an integer between 0 and Move::range */
    unsigned int to_int() const;

private:
    static_assert(range <= USHRT_MAX, "");

    static const unsigned short max_regular_value = range - 3;

    static const unsigned short value_pass = range - 2;

    static const unsigned short value_null = range - 1;

    static const unsigned short value_uninitialized = range;

    unsigned short m_i;

    bool is_initialized() const;
};

inline Move::Move()
{
#if LIBBOARDGAME_DEBUG
    m_i = value_uninitialized;
#endif
}

inline Move::Move(unsigned int i)
{
    LIBBOARDGAME_ASSERT(i < range);
    m_i = static_cast<unsigned short>(i);
}

inline Move& Move::operator=(const Move& mv)
{
    m_i = mv.m_i;
    return *this;
}

inline bool Move::operator==(const Move& mv) const
{
    return m_i == mv.m_i;
}

inline bool Move::operator!=(const Move& mv) const
{
    return ! operator==(mv);
}

inline bool Move::is_initialized() const
{
    return m_i < value_uninitialized;
}

inline bool Move::is_null() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i == value_null;
}

inline bool Move::is_pass() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i == value_pass;
}

inline bool Move::is_regular() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i <= max_regular_value;
}

inline Move Move::null()
{
    return Move(value_null);
}

inline Move Move::pass()
{
    return Move(value_pass);
}

inline unsigned int Move::to_int() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_BASE_MOVE_H
