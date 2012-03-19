//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/Tree.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Tree.h"

#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <boost/algorithm/string.hpp>
#include "libboardgame_sgf/Util.h"

namespace libboardgame_sgf {

using boost::algorithm::ends_with;
using libboardgame_sgf::util::find_root;
using libboardgame_sgf::util::get_go_point_property_value;
using libboardgame_sgf::util::parse_go_move_property_value;
using libboardgame_sgf::util::parse_go_point_property_value;

//-----------------------------------------------------------------------------

Tree::Tree()
{
    init();
}

Tree::~Tree()
{
}

void Tree::append_comment(const Node& node, const string& s)
{
    string comment = get_comment(node);
    if (comment.empty())
        comment = s;
    else
    {
        if (! (ends_with(comment, "\n") || ends_with(comment, "\r")))
            comment += '\n';
        comment += s;
    }
    set_comment(node, s);
}

void Tree::append_comment(const Node& node, const format& f)
{
    append_comment(node, str(f));
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

string Tree::get_comment(const Node& node) const
{
    return node.get_property("C", "");
}

unique_ptr<Node> Tree::get_tree_transfer_ownership()
{
    return move(m_root);
}

bool Tree::has_comment_property(const Node& node, const string& key) const
{
    string comment = get_comment(node);
    istringstream in(comment);
    string line;
    while (getline(in, line))
        if (is_comment_property_line(line, key))
            return true;
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

bool Tree::is_comment_property_line(const string& line, const string& key)
{
    size_t pos = line.find('=');
    if (pos == string::npos)
        return false;
    return (trim_copy(line.substr(0, pos)) == key);
}

void Tree::make_first_child(const Node& node)
{
    const Node* parent = node.get_parent_or_null();
    if (parent != 0 && &parent->get_first_child() != &node)
    {
        non_const(node).make_first_child();
        m_modified = true;
    }
}

void Tree::make_main_variation(const Node& node)
{
    Node* current = &non_const(node);
    while (current->has_parent())
    {
        make_first_child(*current);
        current = &current->get_parent();
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
    string trimmed = trim_copy(s);
    if (trimmed.empty())
        remove_property(node, "C");
    else
        set_property(node, "C", s);
}

void Tree::set_comment(const Node& node, const format& f)
{
    set_comment(node, str(f));
}

void Tree::set_date_today()
{
   time_t t = time(0);
   struct tm* tmp = localtime(&t);
   if (tmp == 0)
       return;
    char date[128];
    if (strftime(date, sizeof(date), "%Y-%m-%d", tmp) == 0)
        return;
    set_date(date);
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
    const Node& parent = node.get_parent();
    non_const(parent).remove_child(non_const(node));
    m_modified = true;
    return parent;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
