//-----------------------------------------------------------------------------
/** @file libpentobi_base/PlayerBase.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_PLAYER_BASE_H
#define LIBPENTOBI_BASE_PLAYER_BASE_H

#include "Board.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

class PlayerBase
{
public:
    virtual ~PlayerBase() = default;

    virtual Move genmove(const Board& bd, Color c) = 0;

    /** Check if the player wants to resign.
        This may only be called after a genmove() and returns true if the
        player wants to resign in the position at the last genmove().
        The default implementation returns false. */
    virtual bool resign() const;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PLAYER_BASE_H
