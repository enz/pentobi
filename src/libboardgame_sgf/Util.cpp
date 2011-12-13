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

const Node* find_next_comment(const Node& node)
{
    const Node* current = get_next_node(node);
    while (current != 0)
    {
        if (has_comment(*current))
            return current;
        current = get_next_node(*current);
    }
    return 0;
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

const Node* get_next_earlier_variation(const Node& node)
{
    const Node* child = &node;
    const Node* current = node.get_parent_or_null();
    while (current != 0 && child->get_sibling() == 0)
    {
        child = current;
        current = current->get_parent_or_null();
    }
    if (current == 0)
        return 0;
    return child->get_sibling();
}

const Node* get_next_node(const Node& node)
{
    const Node* child = node.get_first_child_or_null();
    if (child != 0)
        return child;
    return get_next_earlier_variation(node);
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

bool has_comment(const Node& node)
{
    return node.has_property("C");
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
