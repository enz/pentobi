//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/Util.cpp */
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
#include "libboardgame_util/StringUtil.h"

namespace libboardgame_sgf {
namespace util {

using namespace std;
using libboardgame_util::string_util::get_letter_coord;

//-----------------------------------------------------------------------------

namespace {

void write_node(Writer& writer, const Node& node)
{
    writer.begin_node();
    for (PropertyIterator i(node); i; ++i)
        writer.write_property(i->id, i->values);
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
    if (is_main_variation(node))
        return node;
    const Node* current = &node;
    while (! is_main_variation(*current))
        current = &current->get_parent();
    return current->get_first_child();
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

unsigned int get_depth(const Node& node)
{
    unsigned int depth = 0;
    const Node* current = &node;
    while (current->has_parent())
    {
        current = &current->get_parent();
        ++depth;
    }
    return depth;
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

string get_variation_string(const Node& node)
{
    string result;
    const Node* current = &node;
    unsigned int depth = get_depth(*current);
    while (current->has_parent())
    {
        const Node& parent = current->get_parent();
        if (parent.get_nu_children() > 1)
        {
            unsigned int index = parent.get_child_index(*current);
            if (index > 0)
            {
                ostringstream s;
                s << depth << get_letter_coord(index);
                if (! result.empty())
                    s << '-' << result;
                result = s.str();
            }
        }
        current = &parent;
        --depth;
    }
    return result;
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

void write_tree(ostream& out, const Node& root, bool one_prop_per_line,
                bool one_prop_value_per_line, unsigned int indent)
{
    Writer writer(out, one_prop_per_line, one_prop_value_per_line, indent);
    writer.begin_tree();
    write_node(writer, root);
    writer.end_tree();
}

//-----------------------------------------------------------------------------

} // namespace util
} // namespace libboardgame_sgf
