//-----------------------------------------------------------------------------
/** @file libpentobi_base/TreeUtil.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_TREE_UTIL_H
#define LIBPENTOBI_BASE_TREE_UTIL_H

#include "PentobiTree.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** Get the node with a given move number in the variation of a given node. */
const SgfNode* get_move_node(const PentobiTree& tree, const SgfNode& node,
                             unsigned n);

/** Get the move number at a node.
    Counts the number of moves since the root node or the last node
    that contained setup properties. Invalid moves are ignored. */
unsigned get_move_number(const PentobiTree& tree, const SgfNode& node);

/** Get the number of remaining moves in the current variation.
    Counts the number of moves remaining in the current variation
    until the end of the variation or the next node that contains setup
    properties. Invalid moves are ignored. */
unsigned get_moves_left(const PentobiTree& tree, const SgfNode& node);

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_TREE_UTIL_H
