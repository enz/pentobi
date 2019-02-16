//-----------------------------------------------------------------------------
/** @file libpentobi_base/MoveList.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MOVE_LIST_H
#define LIBPENTOBI_BASE_MOVE_LIST_H

#include "Move.h"
#include "libboardgame_base/ArrayList.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** List that can hold all possible moves, not including Move::null() */
using MoveList = libboardgame_base::ArrayList<Move, Move::range - 1>;

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_MOVE_LIST_H
