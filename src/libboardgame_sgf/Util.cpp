//-----------------------------------------------------------------------------
/** @file sgf/Util.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Util.h"

#include <iostream>
#include <sstream>
#include "InvalidPropertyValue.h"
#include "Node.h"
#include "Writer.h"

namespace libboardgame_sgf {
namespace util {

using namespace std;

//-----------------------------------------------------------------------------

namespace {

void write_node(Writer& writer, const Node& node)
{
    writer.begin_node();
    for (PropertyIterator i(node); i; ++i)
        writer.write_property(i->m_id, i->m_values);
    writer.end_node();
    if (! node.has_children())
        return;
    else if (node.has_single_child())
        write_node(writer, node.get_child());
    else
        for (ChildIterator i(node); i; ++i)
        {
            writer.begin_tree();
            write_node(writer, *i);
            writer.end_tree();
        }
}

} // namespace

//-----------------------------------------------------------------------------

const Node& back_to_main_variation(const Node& node)
{
    const Node* current = &node;
    while (! is_main_variation(*current))
        current = &current->get_parent();
    return *current;
}

const Node& find_root(const Node& node)
{
    const Node* current = &node;
    while (current->has_parent())
        current = &current->get_parent();
    return *current;
}

string get_go_point_property_value(int x, int y, int sz)
{
    ostringstream result;
    result << char('a' + x);
    result << char('a' + (sz - y - 1));
    return result.str();
}

const Node& get_last_node(const Node& node)
{
    const Node* n = &node;
    while (n->has_children())
        n = &n->get_first_child();
    return *n;
}

void get_path_from_root(const Node& node, vector<const Node*>& path)
{
    const Node* current = &node;
    path.assign(1, current);
    while(current->has_parent())
    {
        current = &current->get_parent();
        path.push_back(current);
    }
    reverse(path.begin(), path.end());
}

/** Get a text representation of the variation to a certain node.
    The string contains the number of the child for each node with more
    than one child in the path from the root node to this node.
    The childs are counted starting with 1 and the numbers are separated
    by colons. */
string get_variation_string(const Node& node)
{
    vector<unsigned int> list;
    const Node* current = &node;
    while (current != 0)
    {
        const Node* parent = current->get_parent_or_null();
        if (parent != 0 && parent->get_nu_children() > 1)
        {
            unsigned int index = parent->get_child_index(*current) + 1;
            list.insert(list.begin(), index);
        }
        current = parent;
    }
    ostringstream s;
    for (unsigned int i = 0; i < list.size(); ++i)
    {
        s << list[i];
        if (i < list.size() - 1)
            s << '.';
    }
    return s.str();
}

bool is_main_variation(const Node& node)
{
    const Node* current = &node;
    while (current->has_parent())
    {
        const Node& parent = current->get_parent();
        if (current != &parent.get_first_child())
            return false;
        current = &parent;
    }
    return true;
}

void parse_go_move_property_value(const string& s, int& x, int& y, int sz)
{
    LIBBOARDGAME_ASSERT(sz >= 1);
    LIBBOARDGAME_ASSERT(sz <= 52);
    if (s.size() == 0 || (sz <= 19 && s == "tt"))
    {
        x = -1;
        y = -1;
        return;
    }
    return parse_go_point_property_value(s, x, y, sz);
}

void parse_go_point_property_value(const string& s, int& x, int& y, int sz)
{
    LIBBOARDGAME_ASSERT(sz >= 1);
    LIBBOARDGAME_ASSERT(sz <= 52);
    if (s.size() != 2)
        throw InvalidPropertyValue(s, "Not a valid point");
    char x_char = s[0];
    char y_char = s[1];
    if (x_char >= 'a' && x_char <= 'z')
        x = x_char - 'a';
    else if (x_char >= 'A' && x_char <= 'Z')
        x = x_char - 'A' + 27;
    else
        throw InvalidPropertyValue(s, "Not a valid point");
    if (y_char >= 'a' && y_char <= 'z')
        y = sz - (y_char - 'a') - 1;
    else if (y_char >= 'A' && y_char <= 'Z')
        y = sz - (y_char - 'A' + 27) - 1;
    else
        throw InvalidPropertyValue(s, "Not a valid point");
    if (x < 0 || y < 0 || x >= sz || y >= sz)
        throw InvalidPropertyValue(s, format("Point not on board of size %1%")
                                   % sz);
}

void write_tree(ostream& out, const Node& root, bool one_node_per_line,
                unsigned int indent)
{
    Writer writer(out, one_node_per_line, indent);
    writer.begin_tree();
    write_node(writer, root);
    writer.end_tree();
}

//-----------------------------------------------------------------------------

} // namespace util
} // namespace libboardgame_sgf
