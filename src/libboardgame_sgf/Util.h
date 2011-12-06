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

/** Find next node with a comment in the iteration through complete tree.
    @param node The current node in the iteration.
    @return The next node in the iteration through the complete tree
    after the current node that has a comment. */
const Node* find_next_comment(const Node& node);

const Node& find_root(const Node& node);

/** Get list of nodes from root to a target node.
    @param node The target node.
    @param[out] path The list of nodes. */
void get_path_from_root(const Node& node, vector<const Node*>& path);

const Node& get_last_node(const Node& node);

/** Get next node for iteration through complete tree. */
const Node* get_next_node(const Node& node);

/** Return next variation before this node. */
const Node* get_next_earlier_variation(const Node& node);

/** Get a text representation of the variation to a certain node.
    The string contains the number of the child for each node with more
    than one child in the path from the root node to this node.
    The childs are counted starting with 1 and the numbers are separated
    by colons. */
string get_variation_string(const Node& node);

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

void write_tree(ostream& out, const Node& root, bool one_node_per_line = false,
                unsigned int indent = 0);

//-----------------------------------------------------------------------------

} // namespace util
} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_UTIL_H
