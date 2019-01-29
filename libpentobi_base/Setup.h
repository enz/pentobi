//-----------------------------------------------------------------------------
/** @file libpentobi_base/Setup.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_SETUP_H
#define LIBPENTOBI_BASE_SETUP_H

#include "ColorMap.h"
#include "Move.h"
#include "libboardgame_util/ArrayList.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** Definition of a setup position.
    A setup position consists of a number of pieces that are placed at once
    (in no particular order) on the board and a color to play next. */
struct Setup
{
    /** Maximum number of pieces on board per color. */
    static constexpr unsigned max_pieces = 24;

    using PlacementList = libboardgame_util::ArrayList<Move, max_pieces>;


    Color to_play = Color(0);

    ColorMap<PlacementList> placements;

    void clear();
};

inline void Setup::clear()
{
    to_play = Color(0);
    for_each_color([&](Color c) { placements[c].clear(); });
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_SETUP_H
