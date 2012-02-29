//-----------------------------------------------------------------------------
/** @file libpentobi_base/Setup.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_SETUP_H
#define LIBPENTOBI_BASE_SETUP_H

#include "BoardConst.h"
#include "ColorMap.h"
#include "Move.h"
#include "libboardgame_util/ArrayList.h"

namespace libpentobi_base {

using libboardgame_util::ArrayList;

//-----------------------------------------------------------------------------

/** Definition of a setup position.
    A setup position consists of a number of pieces that are placed at once
    (in no particular order) on the board and a color to play next. */
struct Setup
{
    Color to_play;

    ColorMap<ArrayList<Move,BoardConst::max_pieces>> placements;

    Setup();

    void clear();
};

inline Setup::Setup()
{
    to_play = Color(0);
}

inline void Setup::clear()
{
    to_play = Color(0);
    LIBPENTOBI_FOREACH_COLOR(c, placements[c].clear());
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_SETUP_H
