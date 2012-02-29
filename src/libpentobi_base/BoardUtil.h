//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardUtil.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOARDUTIL_H
#define LIBPENTOBI_BASE_BOARDUTIL_H

#include <iosfwd>
#include "Board.h"

namespace libpentobi_base {
namespace boardutil {

//-----------------------------------------------------------------------------

void dump(const Board& bd, ostream& out);

/** Return the current position as setup.
    Merges all placements from Board::get_setup() and played moved into a
    single setup and sets the setup color to play to the current color to
    play. */
void get_current_position_as_setup(const Board& bd, Setup& setup);

//-----------------------------------------------------------------------------

} // namespace boardutil
} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_BOARDUTIL_H
