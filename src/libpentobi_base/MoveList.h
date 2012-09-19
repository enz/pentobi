//-----------------------------------------------------------------------------
/** @file libpentobi_base/MoveList.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MOVE_LIST_H
#define LIBPENTOBI_BASE_MOVE_LIST_H

#include "Move.h"
#include "libboardgame_util/ArrayList.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

typedef ArrayList<Move,Move::range> MoveList;

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_MOVE_LIST_H
