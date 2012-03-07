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
    virtual ~Player() throw();

    virtual Move genmove(const Board& bd, Color c) = 0;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PLAYER_H
