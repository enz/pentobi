//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardUpdater.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOARD_UPDATER_H
#define LIBPENTOBI_BASE_BOARD_UPDATER_H

#include "Board.h"
#include "PentobiTree.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** Updates a board state to a node in a game tree. */
class BoardUpdater
{
public:
    /** Update the board to a node.
        @throws Exception if tree contains invalid properties, moves that play
        the same piece twice or other conditions that prevent the updater to
        update the board to the given node. */
    void update(Board& bd, const PentobiTree& tree, const SgfNode& node);

private:
    /** Local variable reused for efficiency. */
    vector<const SgfNode*> m_path;
};

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_BOARD_UPDATER_H
