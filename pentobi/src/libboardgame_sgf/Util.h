//-----------------------------------------------------------------------------
/** @file sgf/Util.h */
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

const Node& find_root(const Node& node);

vector<const Node*> get_path_from_root(const Node& node);

const Node& get_last_node(const Node& node);

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

void write_tree(ostream& out, const Node& root, bool one_node_per_line = false,
                unsigned int indent = 0);

//-----------------------------------------------------------------------------

} // namespace util
} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_UTIL_H
