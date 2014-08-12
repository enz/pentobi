//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/Tree.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Tree.h"

#include <ctime>
#include <cstdio>
#include <cstdlib>
#include "libboardgame_sgf/Util.h"

namespace libboardgame_sgf {

using libboardgame_sgf::util::find_root;

//-----------------------------------------------------------------------------

Tree::Tree()
{
    init();
}

Tree::~Tree()
{
}

bool Tree::contains(const Node& node) const
{
    return &find_root(node) == &get_root();
}

const Node& Tree::create_new_child(const Node& node)
{
    m_modified = true;
    return non_const(node).create_new_child();
}

void Tree::delete_all_variations()
{
    auto node = &get_root();
    while (node != nullptr)
    {
        non_const(*node).delete_variations();
        node = node->get_first_child_or_null();
    }
}

string Tree::get_comment(const Node& node) const
{
    return node.get_property("C", "");
}

string Tree::get_date_today()
{
    time_t t = time(nullptr);
    auto tmp = localtime(&t);
    if (tmp == nullptr)
        return "?";
    char date[128];
    strftime(date, sizeof(date), "%Y-%m-%d", tmp);
    return date;
}

unique_ptr<Node> Tree::get_tree_transfer_ownership()
{
    return move(m_root);
}

bool Tree::has_variations() const
{
    auto node = m_root.get();
    while (node != nullptr)
    {
        if (node->get_sibling() != nullptr)
            return true;
        node = node->get_first_child_or_null();
    }
    return false;
}

void Tree::init()
{
    unique_ptr<Node> root(new Node());
    m_root = move(root);
    m_modified = false;
}

void Tree::init(unique_ptr<Node>& root)
{
    m_root = move(root);
    m_modified = false;
}

void Tree::make_first_child(const Node& node)
{
    auto parent = node.get_parent_or_null();
    if (parent != nullptr && &parent->get_first_child() != &node)
    {
        non_const(node).make_first_child();
        m_modified = true;
    }
}

void Tree::make_main_variation(const Node& node)
{
    auto current = &non_const(node);
    while (current->has_parent())
    {
        make_first_child(*current);
        current = &current->get_parent();
    }
}

void Tree::make_root(const Node& node)
{
    if (&node == &get_root())
        return;
    LIBBOARDGAME_ASSERT(contains(node));
    auto& parent = node.get_parent();
    unique_ptr<Node> new_root = non_const(parent).remove_child(non_const(node));
    m_root = move(new_root);
    m_modified = true;
}

bool Tree::move_property_to_front(const Node& node, const string& id)
{
    PropertyIterator first(node);
    if (first && (*first).id != id && node.has_property(id))
        m_modified = true;
    return non_const(node).move_property_to_front(id);
}

void Tree::move_down(const Node& node)
{
    if (node.get_sibling())
    {
        non_const(node).move_down();
        m_modified = true;
    }
}

void Tree::move_up(const Node& node)
{
    auto parent = node.get_parent_or_null();
    if (parent != nullptr && &parent->get_first_child() != &node)
    {
        non_const(node).move_up();
        m_modified = true;
    }
}

void Tree::remove_move_annotation(const Node& node)
{
    remove_property(node, "BM");
    remove_property(node, "DO");
    remove_property(node, "IT");
    remove_property(node, "TE");
}

bool Tree::remove_property(const Node& node, const string& id)
{
    bool prop_existed = non_const(node).remove_property(id);
    if (prop_existed)
        m_modified = true;
    return prop_existed;
}

void Tree::set_application(const string& name, const string& version)
{
    if (version.empty())
        set_property(get_root(), "AP", name);
    else
        set_property(get_root(), "AP", name + ":" + version);
}

void Tree::set_property(const Node& node, const string& id, const char* value)
{
    bool was_changed = non_const(node).set_property(id, value);
    if (was_changed)
        m_modified = true;
}

void Tree::set_bad_move(const Node& node, double value)
{
    remove_move_annotation(node);
    set_property(node, "BM", value);
}

void Tree::set_comment(const Node& node, const string& s)
{
    string trimmed = trim(s);
    if (trimmed.empty())
        remove_property(node, "C");
    else
        set_property(node, "C", s);
}

void Tree::set_date_today()
{
    set_date(get_date_today());
}

void Tree::set_doubtful_move(const Node& node)
{
    remove_move_annotation(node);
    set_property(node, "DO", "");
}

void Tree::set_good_move(const Node& node, double value)
{
    remove_move_annotation(node);
    set_property(node, "TE", value);
}

void Tree::set_interesting_move(const Node& node)
{
    remove_move_annotation(node);
    set_property(node, "IT", "");
}

const Node& Tree::truncate(const Node& node)
{
    LIBBOARDGAME_ASSERT(node.has_parent());
    auto& parent = node.get_parent();
    non_const(parent).remove_child(non_const(node));
    m_modified = true;
    return parent;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
