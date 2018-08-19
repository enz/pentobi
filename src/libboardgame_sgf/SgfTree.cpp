//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/SgfTree.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SgfTree.h"

#include <ctime>
#include <cstdio>
#include <cstdlib>
#include "SgfUtil.h"
#include "libboardgame_util/StringUtil.h"

namespace libboardgame_sgf {

using libboardgame_sgf::find_root;
using libboardgame_util::trim;

//-----------------------------------------------------------------------------

SgfTree::SgfTree()
{
    SgfTree::init();
}

bool SgfTree::contains(const SgfNode& node) const
{
    return &find_root(node) == &get_root();
}

const SgfNode& SgfTree::create_new_child(const SgfNode& node)
{
    m_modified = true;
    return non_const(node).create_new_child();
}

void SgfTree::delete_all_variations()
{
    auto node = &get_root();
    do
    {
        delete_variations(*node);
        node = node->get_first_child_or_null();
    }
    while (node != nullptr);
}

void SgfTree::delete_variations(const SgfNode& node)
{
    if (node.get_nu_children() <= 1)
        return;
    non_const(node).delete_variations();
    m_modified = true;
}

double SgfTree::get_bad_move(const SgfNode& node)
{
    return node.parse_property<double>("BM", 0);
}

string SgfTree::get_comment(const SgfNode& node) const
{
    return node.get_property("C", "");
}

string SgfTree::get_date_today()
{
    time_t t = time(nullptr);
    auto tmp = localtime(&t);
    if (tmp == nullptr)
        return "?";
    char date[128];
    strftime(date, sizeof(date), "%Y-%m-%d", tmp);
    return date;
}

double SgfTree::get_good_move(const SgfNode& node)
{
    return node.parse_property<double>("TE", 0);
}

unique_ptr<SgfNode> SgfTree::get_tree_transfer_ownership()
{
    return move(m_root);
}

bool SgfTree::has_variations() const
{
    auto node = m_root.get();
    do
    {
        if (node->get_sibling() != nullptr)
            return true;
        node = node->get_first_child_or_null();
    }
    while (node != nullptr);
    return false;
}

void SgfTree::init()
{
    auto root = make_unique<SgfNode>();
    m_root = move(root);
    m_modified = false;
}

void SgfTree::init(unique_ptr<SgfNode>& root)
{
    m_root = move(root);
    m_modified = false;
}

bool SgfTree::is_doubtful_move(const SgfNode& node)
{
    return node.has_property("DO");
}

bool SgfTree::is_interesting_move(const SgfNode& node)
{
    return node.has_property("IT");
}

void SgfTree::make_first_child(const SgfNode& node)
{
    auto parent = node.get_parent_or_null();
    if (parent != nullptr && &parent->get_first_child() != &node)
    {
        non_const(node).make_first_child();
        m_modified = true;
    }
}

void SgfTree::make_main_variation(const SgfNode& node)
{
    auto current = &non_const(node);
    while (current->has_parent())
    {
        make_first_child(*current);
        current = &current->get_parent();
    }
}

void SgfTree::make_root(const SgfNode& node)
{
    if (&node == &get_root())
        return;
    LIBBOARDGAME_ASSERT(contains(node));
    auto& parent = node.get_parent();
    unique_ptr<SgfNode> new_root = non_const(parent).remove_child(non_const(node));
    m_root = move(new_root);
    m_modified = true;
}

void SgfTree::move_property_to_front(const SgfNode& node, const string& id)
{
    if (non_const(node).move_property_to_front(id))
        m_modified = true;
}

void SgfTree::move_down(const SgfNode& node)
{
    if (node.get_sibling() != nullptr)
    {
        non_const(node).move_down();
        m_modified = true;
    }
}

void SgfTree::move_up(const SgfNode& node)
{
    auto parent = node.get_parent_or_null();
    if (parent != nullptr && &parent->get_first_child() != &node)
    {
        non_const(node).move_up();
        m_modified = true;
    }
}

void SgfTree::remove_move_annotation(const SgfNode& node)
{
    remove_property(node, "BM");
    remove_property(node, "DO");
    remove_property(node, "IT");
    remove_property(node, "TE");
}

bool SgfTree::remove_property(const SgfNode& node, const string& id)
{
    bool prop_existed = non_const(node).remove_property(id);
    if (prop_existed)
        m_modified = true;
    return prop_existed;
}

void SgfTree::set_application(const string& name, const string& version)
{
    if (version.empty())
        set_property(get_root(), "AP", name);
    else
        set_property(get_root(), "AP", name + ":" + version);
}

void SgfTree::set_property(const SgfNode& node, const string& id, const char* value)
{
    bool was_changed = non_const(node).set_property(id, value);
    if (was_changed)
        m_modified = true;
}

void SgfTree::set_property_remove_empty(const SgfNode& node, const string& id,
                                        const string& value)
{
    string trimmed = trim(value);
    if (trimmed.empty())
        remove_property(node, id);
    else
        set_property(node, id, value);
}

void SgfTree::set_bad_move(const SgfNode& node, double value)
{
    remove_move_annotation(node);
    set_property(node, "BM", value);
}

void SgfTree::set_comment(const SgfNode& node, const string& s)
{
    set_property_remove_empty(node, "C", s);
}

void SgfTree::set_date_today()
{
    set_date(get_date_today());
}

void SgfTree::set_doubtful_move(const SgfNode& node)
{
    remove_move_annotation(node);
    set_property(node, "DO", "");
}

void SgfTree::set_good_move(const SgfNode& node, double value)
{
    remove_move_annotation(node);
    set_property(node, "TE", value);
}

void SgfTree::set_interesting_move(const SgfNode& node)
{
    remove_move_annotation(node);
    set_property(node, "IT", "");
}

const SgfNode& SgfTree::truncate(const SgfNode& node)
{
    auto& parent = node.get_parent();
    non_const(parent).remove_child(non_const(node));
    m_modified = true;
    return parent;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
