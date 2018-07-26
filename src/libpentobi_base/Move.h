//-----------------------------------------------------------------------------
/** @file libpentobi_base/Move.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MOVE_H
#define LIBPENTOBI_BASE_MOVE_H

#include <cstdint>
#include "libboardgame_util/Assert.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

class Move
{
public:
    /** Integer type used internally in this class to store a move.
        This class is optimized for size not for speed because there are
        large precomputed data structures that store moves and move lists.
        Therefore it uses uint_least16_t, not uint_fast16_t. */
    using IntType = uint_least16_t;

    static const IntType onboard_moves_classic = 30433;

    static const IntType onboard_moves_trigon = 32131;

    static const IntType onboard_moves_trigon_3 = 24859;

    static const IntType onboard_moves_duo = 13729;

    static const IntType onboard_moves_junior = 7217;

    static const IntType onboard_moves_nexos = 15157;

    static const IntType onboard_moves_callisto = 9433;

    static const IntType onboard_moves_callisto_2 = 4265;

    static const IntType onboard_moves_callisto_3 = 6885;

    static const IntType onboard_moves_gembloq = 31254;

    static const IntType onboard_moves_gembloq_2 = 15018;

    static const IntType onboard_moves_gembloq_3 = 23518;

    /** Integer range of moves.
        The maximum is given by the number of on-board moves in any game
        variant, plus a null move. */
    static const IntType range = onboard_moves_trigon + 1;

    static Move null();

    Move();

    explicit Move(IntType i);

    bool operator==(Move mv) const;

    bool operator!=(Move mv) const;

    bool operator<(Move mv) const;

    bool is_null() const;

    /** Return move as an integer between 0 and Move::range */
    IntType to_int() const;

private:
    static const IntType value_uninitialized = range;

    IntType m_i;

    bool is_initialized() const;
};

inline Move::Move()
{
#ifdef LIBBOARDGAME_DEBUG
    m_i = value_uninitialized;
#endif
}

inline Move::Move(IntType i)
{
    LIBBOARDGAME_ASSERT(i < range);
    m_i = i;
}

inline bool Move::operator==(Move mv) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(mv.is_initialized());
    return m_i == mv.m_i;
}

inline bool Move::operator!=(Move mv) const
{
    return ! operator==(mv);
}

inline bool Move::operator<(Move mv) const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    LIBBOARDGAME_ASSERT(mv.is_initialized());
    return m_i < mv.m_i;
}

inline bool Move::is_initialized() const
{
    return m_i < value_uninitialized;
}

inline bool Move::is_null() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i == 0;
}

inline Move Move::null()
{
    return Move(0);
}

inline Move::IntType Move::to_int() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_BASE_MOVE_H
