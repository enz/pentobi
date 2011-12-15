//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardUpdater.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOARD_UPDATER_H
#define LIBPENTOBI_BASE_BOARD_UPDATER_H

#include "Board.h"
#include "Tree.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** Updates a board state to a node in a game tree. */
class BoardUpdater
{
public:
    BoardUpdater(const Tree& tree, Board& bd);

    /** Update the board to a node.
        @throws Exception if tree contains invalid properties. */
    void update(const Node& node);

private:
    const Tree& m_tree;

    Board& m_bd;

    /** Local variable reused for efficiency. */
    vector<const Node*> m_path;
};

inline BoardUpdater::BoardUpdater(const Tree& tree, Board& bd)
  : m_tree(tree),
    m_bd(bd)
{
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_BOARD_UPDATER_H
