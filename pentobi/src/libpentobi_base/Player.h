//-----------------------------------------------------------------------------
/** @file libpentobi_base/Player.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_PLAYER_H
#define LIBPENTOBI_BASE_PLAYER_H

#include "Board.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

class Player
{
public:
    Player(const Board& bd);

    virtual ~Player() throw();

    virtual Move genmove(Color c) = 0;

protected:
    const Board& m_bd;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PLAYER_H
