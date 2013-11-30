//-----------------------------------------------------------------------------
/** @file libpentobi_base/MoveMarker.h
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MOVE_MARKER_H
#define LIBPENTOBI_BASE_MOVE_MARKER_H

#include "Move.h"
#include "libboardgame_util/BitMarker.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

typedef libboardgame_util::BitMarker<Move> MoveMarker;

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_MOVE_MARKER_H
