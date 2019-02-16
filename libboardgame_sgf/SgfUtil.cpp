//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/SgfUtil.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "SgfUtil.h"

#include <algorithm>
#include <sstream>
#include "libboardgame_base/StringUtil.h"

namespace libboardgame_sgf {

using libboardgame_base::get_letter_coord;

//-----------------------------------------------------------------------------

const SgfNode& back_to_main_variation(const SgfNode& node)
{
    if (is_main_variation(node))
        return node;
    auto current = &node;
    while (! is_main_variation(*current))
        current = &current->get_parent();
    return current->get_first_child();
}

const SgfNode& beginning_of_branch(const SgfNode& node)
{
    auto current = node.get_parent_or_null();
    if (current == nullptr)
        return node;
    while (true)
    {
        auto parent = current->get_parent_or_null();
        if (parent == nullptr || ! parent->has_single_child())
            break;
        current = parent;
    }
    return *current;
}

const SgfNode* find_next_comment(const SgfNode& node)
{
    auto current = get_next_node(node);
    while (current != nullptr)
    {
        if (has_comment(*current))
            return current;
        current = get_next_node(*current);
    }
    return nullptr;
}

const SgfNode& find_root(const SgfNode& node)
{
    auto current = &node;
    while (current->has_parent())
        current = &current->get_parent();
    return *current;
}

const SgfNode& get_last_node(const SgfNode& node)
{
    auto n = &node;
    while (n->has_children())
        n = &n->get_first_child();
    return *n;
}

unsigned get_depth(const SgfNode& node)
{
    unsigned depth = 0;
    auto current = &node;
    while (current->has_parent())
    {
        current = &current->get_parent();
        ++depth;
    }
    return depth;
}

const SgfNode* get_next_earlier_variation(const SgfNode& node)
{
    auto child = &node;
    auto current = node.get_parent_or_null();
    while (current != nullptr && (child->get_sibling() == nullptr))
    {
        child = current;
        current = current->get_parent_or_null();
    }
    if (current == nullptr)
        return nullptr;
    return child->get_sibling();
}

const SgfNode* get_next_node(const SgfNode& node)
{
    auto child = node.get_first_child_or_null();
    if (child != nullptr)
        return child;
    return get_next_earlier_variation(node);
}

void get_path_from_root(const SgfNode& node, vector<const SgfNode*>& path)
{
    auto current = &node;
    path.assign(1, current);
    while(current->has_parent())
    {
        current = &current->get_parent();
        path.push_back(current);
    }
    reverse(path.begin(), path.end());
}

string get_variation_string(const SgfNode& node)
{
    string result;
    auto current = &node;
    unsigned depth = get_depth(*current);
    while (current->has_parent())
    {
        auto& parent = current->get_parent();
        if (parent.get_nu_children() > 1)
        {
            unsigned index = parent.get_child_index(*current);
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

bool has_comment(const SgfNode& node)
{
    return node.has_property("C");
}

bool has_earlier_variation(const SgfNode& node)
{
    auto current = node.get_parent_or_null();
    if (current == nullptr)
        return false;
    while (true)
    {
        auto parent = current->get_parent_or_null();
        if (parent == nullptr)
            return false;
        if (! parent->has_single_child())
            return true;
        current = parent;
    }
}

bool is_empty(const SgfTree& tree)
{
    auto& root = tree.get_root();
    if (root.has_children())
        return false;
    for (auto& p : root.get_properties())
    {
        auto& id = p.id;
        if (id != "GM" && id != "CA" && id != "AP" && id != "DT")
            return false;
    }
    return true;
}

bool is_main_variation(const SgfNode& node)
{
    auto current = &node;
    while (current->has_parent())
    {
        auto& parent = current->get_parent();
        if (current != &parent.get_first_child())
            return false;
        current = &parent;
    }
    return true;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
