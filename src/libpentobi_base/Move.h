//-----------------------------------------------------------------------------
/** @file libpentobi_base/Move.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MOVE_H
#define LIBPENTOBI_BASE_MOVE_H

#include "libboardgame_util/Assert.h"
#include "libboardgame_util/Exception.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_util::Exception;

//-----------------------------------------------------------------------------

class Move
{
public:
    /** Integer range of moves.
        The maximum is given by game variant Trigon, which has 37334 regular
        moves plus a pass and a null move. */
    static const unsigned int range = 37334 + 1 + 1;

    static Move pass();

    static Move null();

    Move();

    explicit Move(unsigned int i);

    Move& operator=(const Move& mv);

    bool operator==(const Move& mv) const;

    bool operator!=(const Move& mv) const;

    bool is_pass() const;

    bool is_null() const;

    /** Return move as an integer between 0 and Move::range */
    unsigned int to_int() const;

private:
    static const unsigned int value_pass = range - 2;

    static const unsigned int value_null = range - 1;

    static const unsigned int value_uninitialized = range;

    unsigned int m_i;

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
    m_i = i;
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

inline bool Move::is_pass() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i == value_pass;
}

inline bool Move::is_null() const
{
    LIBBOARDGAME_ASSERT(is_initialized());
    return m_i == value_null;
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
