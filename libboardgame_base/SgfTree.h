//-----------------------------------------------------------------------------
/** @file libboardgame_base/SgfTree.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_SGF_TREE_H
#define LIBBOARDGAME_BASE_SGF_TREE_H

#include "SgfNode.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

/** SGF tree.
    Tree structure of the tree can only be manipulated through member functions
    to guarantee a consistent tree structure. Therefore the user is given
    only const references to nodes and non-const functions of nodes can only
    be called through wrapper functions of the tree (in which case the user
    passes in a const reference to the node as an identifier for the node). */
class SgfTree
{
public:
    SgfTree();

    virtual ~SgfTree() = default;


    virtual void init();

    /** Initialize from an existing SGF tree.
        @param root The root node of the SGF tree; the ownership is transferred
        to this class. */
    virtual void init(unique_ptr<SgfNode>& root);

    /** Get the root node and transfer the ownership to the caller. */
    unique_ptr<SgfNode> get_tree_transfer_ownership();

    /** Check if the tree was modified since the construction or the last call
        to init() or clear_modified() */
    bool is_modified() const { return m_modified; }

    void set_modified(bool is_modified = true) { m_modified = is_modified; }

    void clear_modified() { m_modified = false; }

    const SgfNode& get_root() const { return *m_root; }

    const SgfNode& create_new_child(const SgfNode& node);

    /** Truncate a node and its subtree from the tree.
        Calling this function deletes the node that is to be truncated and its
        complete subtree.
        @pre node.has_parent()
        @param node The node to be truncated.
        @return The parent of the truncated node. */
    const SgfNode& truncate(const SgfNode& node);

    /** Delete all children but the first. */
    void delete_variations(const SgfNode& node);

    /** Delete all variations but the main variation. */
    void delete_all_variations();

    /** Make a node the first child of its parent. */
    void make_first_child(const SgfNode& node);

    /** Make a node switch place with its previous sibling (if it is not
        already the first child). */
    void move_up(const SgfNode& node);

    /** Make a node switch place with its next sibling (if it is not
        already the last child). */
    void move_down(const SgfNode& node);

    /** Make a node the root node of the tree.
        All nodes that are not the given node or in the subtree below it are
        deleted. Note that this operation in general creates a semantically
        invalid tree (e.g. missing GM or CA property in the new root). You need
        to add those after this function. In general, you will also have to
        examine the nodes in the path to the node in the original tree and then
        make the tree valid again after calling make_root(). Typically, you
        will have to look at the moves played before this node and convert them
        into setup properties to add to the new root such that the board
        position at this node is the same as originally. */
    void make_root(const SgfNode& node);

    void make_main_variation(const SgfNode& node);

    bool contains(const SgfNode& node) const;

    template<typename T>
    void set_property(const SgfNode& node, const string& id, const T& value);

    void set_property(const SgfNode& node, const string& id, const char* value);

    template<typename T>
    void set_property(const SgfNode& node, const string& id,
                      const vector<T>& values);

    void set_property_remove_empty(const SgfNode& node,
                                   const string& id, const string& value);

    bool remove_property(const SgfNode& node, const string& id);

    void move_property_to_front(const SgfNode& node, const string& id);

    /** See Node::remove_children() */
    void remove_children(const SgfNode& node);

    void append(const SgfNode& node, unique_ptr<SgfNode> child);

    /** Get comment.
        @return The comment, or an empty string if the node contains no
        comment. */
    string get_comment(const SgfNode& node) const;

    void set_comment(const SgfNode& node, const string& s);

    void remove_move_annotation(const SgfNode& node);

    static double get_good_move(const SgfNode& node);

    void set_good_move(const SgfNode& node, double value = 1);

    static double get_bad_move(const SgfNode& node);

    void set_bad_move(const SgfNode& node, double value = 1);

    static bool is_doubtful_move(const SgfNode& node);

    void set_doubtful_move(const SgfNode& node);

    static bool is_interesting_move(const SgfNode& node);

    void set_interesting_move(const SgfNode& node);

    void set_charset(const string& charset);

    void set_application(const string& name, const string& version = {});

    string get_date() const { return m_root->get_property("DT", ""); }

    void set_date(const string& date);

    /** Get today's date in format YYYY-MM-DD as required by DT property. */
    static string get_date_today();

    void set_date_today();

    string get_event() const { return m_root->get_property("EV", ""); }

    void set_event(const string& event);

    string get_round() const { return m_root->get_property("RO", ""); }

    void set_round(const string& round);

    string get_time() const { return m_root->get_property("TM", ""); }

    void set_time(const string& time);

    bool has_variations() const;

private:
    bool m_modified;

    unique_ptr<SgfNode> m_root;

    SgfNode& non_const(const SgfNode& node);
};

inline void SgfTree::append(const SgfNode& node, unique_ptr<SgfNode> child)
{
    if (child)
        m_modified = true;
    non_const(node).append(move(child));
}

inline SgfNode& SgfTree::non_const(const SgfNode& node)
{
    LIBBOARDGAME_ASSERT(contains(node));
    return const_cast<SgfNode&>(node);
}

inline void SgfTree::remove_children(const SgfNode& node)
{
    if (node.has_children())
        m_modified = true;
    non_const(node).remove_children();
}

inline void SgfTree::set_charset(const string& charset)
{
    set_property_remove_empty(get_root(), "CA", charset);
}

inline void SgfTree::set_date(const string& date)
{
    set_property_remove_empty(get_root(), "DT", date);
}

inline void SgfTree::set_event(const string& event)
{
    set_property_remove_empty(get_root(), "EV", event);
}

template<typename T>
void SgfTree::set_property(const SgfNode& node, const string& id, const T& value)
{
    bool was_changed = non_const(node).set_property(id, value);
    if (was_changed)
        m_modified = true;
}

template<typename T>
void SgfTree::set_property(const SgfNode& node, const string& id,
                        const vector<T>& values)
{
    bool was_changed = non_const(node).set_property(id, values);
    if (was_changed)
        m_modified = true;
}

inline void SgfTree::set_round(const string& round)
{
    set_property_remove_empty(get_root(), "RO", round);
}

inline void SgfTree::set_time(const string& time)
{
    set_property_remove_empty(get_root(), "TM", time);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_SGF_TREE_H
