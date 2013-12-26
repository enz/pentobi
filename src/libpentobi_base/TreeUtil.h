//-----------------------------------------------------------------------------
/** @file libpentobi_base/TreeUtil.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_TREE_UTIL_H
#define LIBPENTOBI_BASE_TREE_UTIL_H

#include "Tree.h"

namespace libpentobi_base {
namespace tree_util {

//-----------------------------------------------------------------------------

/** Get the move number at a node.
    Counts the number of non-pass moves since the root node or the last node
    that contained setup properties. Invalid moves are ignored. */
unsigned get_move_number(const Tree& tree, const Node& node);

/** Get the number of remaining moves in the current variation.
    Counts the number of non-pass moves remaining in the current variation
    until the end of the variation or the next node that contains setup
    properties. Invalid moves are ignored. */
unsigned get_moves_left(const Tree& tree, const Node& node);

//-----------------------------------------------------------------------------

} // namespace tree_util
} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_TREE_UTIL_H
