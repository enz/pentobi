//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/Util.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_UTIL_H
#define LIBBOARDGAME_SGF_UTIL_H

#include <iosfwd>
#include <string>
#include "Node.h"

namespace libboardgame_sgf {
namespace util {

using namespace std;

//-----------------------------------------------------------------------------

/** Return the last node in the current variation that had a sibling. */
const Node& beginning_of_branch(const Node& node);

/** Find next node with a comment in the iteration through complete tree.
    @param node The current node in the iteration.
    @return The next node in the iteration through the complete tree
    after the current node that has a comment. */
const Node* find_next_comment(const Node& node);

const Node& find_root(const Node& node);

/** Get the depth of a node.
    The root node has depth 0. */
unsigned int get_depth(const Node& node);

/** Get list of nodes from root to a target node.
    @param node The target node.
    @param[out] path The list of nodes. */
void get_path_from_root(const Node& node, vector<const Node*>& path);

const Node& get_last_node(const Node& node);

/** Get next node for iteration through complete tree. */
const Node* get_next_node(const Node& node);

/** Return next variation before this node. */
const Node* get_next_earlier_variation(const Node& node);

/** Get a text representation of the variation of a certain node.
    The variation string is a sequence of X.Y for each branching into a
    variation that is not the first child since the root node separated by
    commas, with X being the depth of the child node (starting at 0, and
    therefore equivalent to the move number if there are no non-root nodes
    without moves) and Y being the number of the child (starting at 1). */
string get_variation_string(const Node& node);

/** Check if any previous node had a sibling. */
bool has_earlier_variation(const Node& node);

bool is_main_variation(const Node& node);

const Node& back_to_main_variation(const Node& node);

/** Construct a property value as used for points in Go. */
string get_go_point_property_value(int x, int y, int sz);

/** Parse a property value as used for points in Go.
    @param s The property value
    @param[out] x The x coordinate or -1, if the value is a pass move encoding
    compatible with FF[3] ("tt" for boards sizes less or equal 19).
    @param[out] y The y coordinate or -1, if the value is a pass move encoding
    compatible with FF[3] ("tt" for boards sizes less or equal 19).
    @param sz The board size
    @throws InvalidPropertyValue */
void parse_go_move_property_value(const string& s, int& x, int& y, int sz);

void parse_go_point_property_value(const string& s, int& x, int& y, int sz);

bool has_comment(const Node& node);

//-----------------------------------------------------------------------------

} // namespace util
} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_UTIL_H
