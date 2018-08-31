//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/SgfUtil.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_SGF_UTIL_H
#define LIBBOARDGAME_SGF_SGF_UTIL_H

#include <string>
#include "SgfTree.h"

namespace libboardgame_sgf {

using namespace std;

//-----------------------------------------------------------------------------

/** Return the last node in the current variation that had a sibling. */
const SgfNode& beginning_of_branch(const SgfNode& node);

/** Find next node with a comment in the iteration through complete tree.
    @param node The current node in the iteration.
    @return The next node in the iteration through the complete tree
    after the current node that has a comment. */
const SgfNode* find_next_comment(const SgfNode& node);

const SgfNode& find_root(const SgfNode& node);

/** Get the depth of a node.
    The root node has depth 0. */
unsigned get_depth(const SgfNode& node);

/** Get list of nodes from root to a target node.
    @param node The target node.
    @param[out] path The list of nodes. */
void get_path_from_root(const SgfNode& node, vector<const SgfNode*>& path);

const SgfNode& get_last_node(const SgfNode& node);

/** Get a string representation of move annotation properties. */
const char* get_move_annotation(const SgfNode& node);

/** Get next node for iteration through complete tree. */
const SgfNode* get_next_node(const SgfNode& node);

/** Return next variation before this node. */
const SgfNode* get_next_earlier_variation(const SgfNode& node);

/** Get a text representation of the variation of a certain node.
    The variation string is a sequence of X.Y for each branching into a
    variation that is not the first child since the root node separated by
    commas, with X being the depth of the child node (starting at 0, and
    therefore equivalent to the move number if there are no non-root nodes
    without moves) and Y being the number of the child (starting at 1). */
string get_variation_string(const SgfNode& node);

/** Check if any previous node had a sibling. */
bool has_earlier_variation(const SgfNode& node);

bool is_main_variation(const SgfNode& node);

const SgfNode& back_to_main_variation(const SgfNode& node);

bool has_comment(const SgfNode& node);

/** Check if a tree doesn't contain nodes apart from the root node
    or properties apart from some trivial properties (GM, CA, AP or DT) */
bool is_empty(const SgfTree& tree);

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_SGF_UTIL_H
