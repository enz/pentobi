//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/Tree.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_TREE_H
#define LIBBOARDGAME_SGF_TREE_H

#include "libboardgame_sgf/Node.h"
#include "libboardgame_util/StringUtil.h"

namespace libboardgame_sgf {

using namespace std;
using libboardgame_util::trim;

//-----------------------------------------------------------------------------

/** SGF tree.
    Tree structure of the tree can only be manipulated through member functions
    to guarantee a consistent tree structure. Therefore the user is given
    only const references to nodes and non-const functions of nodes can only
    be called through wrapper functions of the tree (in which case the user
    passes in a const reference to the node as an identifier for the node). */
class Tree
{
public:
    Tree();

    virtual ~Tree();

    virtual void init();

    /** Initialize from an existing SGF tree.
        @param root The root node of the SGF tree; the ownership is transfered
        to this class. */
    virtual void init(unique_ptr<Node>& root);

    /** Get the root node and transfer the ownership to the caller. */
    unique_ptr<Node> get_tree_transfer_ownership();

    /** Check if the tree was modified since the construction or the last call
        to init() or clear_modified() */
    bool is_modified() const;

    void set_modified();

    void clear_modified();

    const Node& get_root() const;

    const Node& create_new_child(const Node& node);

    /** Truncate a node and its subtree from the tree.
        Calling this function deletes the node that is to be truncated and its
        complete subtree.
        @pre node.has_parent()
        @param node The node to be truncated.
        @return The parent of the truncated node. */
    const Node& truncate(const Node& node);

    /** Delete all variations but the main variation. */
    void delete_all_variations();

    /** Make a node the first child of its parent. */
    void make_first_child(const Node& node);

    /** Make a node switch place with its previous sibling (if it is not
        already the first child). */
    void move_up(const Node& node);

    /** Make a node switch place with its next sibling (if it is not
        already the last child). */
    void move_down(const Node& node);

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
    void make_root(const Node& node);

    void make_main_variation(const Node& node);

    bool contains(const Node& node) const;

    template<typename T>
    void set_property(const Node& node, const string& id, const T& value);

    void set_property(const Node& node, const string& id, const char* value);

    template<typename T>
    void set_property(const Node& node, const string& id,
                      const vector<T>& values);

    bool remove_property(const Node& node, const string& id);

    bool move_property_to_front(const Node& node, const string& id);

    /** See Node::remove_children() */
    unique_ptr<Node> remove_children(const Node& node);

    void append(const Node& node, unique_ptr<Node> child);

    /** Get comment.
        @return The comment, or an empty string if the node contains no
        comment. */
    string get_comment(const Node& node) const;

    void set_comment(const Node& node, const string& s);

    void append_comment(const Node& node, const string& s);

    void remove_move_annotation(const Node& node);

    double get_good_move(const Node& node) const;

    void set_good_move(const Node& node, double value = 1);

    double get_bad_move(const Node& node) const;

    void set_bad_move(const Node& node, double value = 1);

    bool is_doubtful_move(const Node& node) const;

    void set_doubtful_move(const Node& node);

    bool is_interesting_move(const Node& node) const;

    void set_interesting_move(const Node& node);

    void set_charset(const string& charset);

    void set_application(const string& name, const string& version = "");

    string get_date() const;

    void set_date(const string& date);

    /** Get today's date in format YYYY-MM-DD as required by DT property. */
    static string get_date_today();

    void set_date_today();

    string get_event() const;

    void set_event(const string& event);

    string get_round() const;

    void set_round(const string& date);

    string get_time() const;

    void set_time(const string& time);

    bool has_variations() const;

private:
    bool m_modified;

    unique_ptr<Node> m_root;

    Node& non_const(const Node& node);
};

inline void Tree::append(const Node& node, unique_ptr<Node> child)
{
    if (child)
        m_modified = true;
    non_const(node).append(move(child));
}

inline void Tree::clear_modified()
{
    m_modified = false;
}

inline double Tree::get_bad_move(const Node& node) const
{
    return node.get_property<double>("BM", 0);
}

inline string Tree::get_date() const
{
    return m_root->get_property("DT", "");
}

inline string Tree::get_event() const
{
    return m_root->get_property("EV", "");
}

inline double Tree::get_good_move(const Node& node) const
{
    return node.get_property<double>("TE", 0);
}

inline bool Tree::is_modified() const
{
    return m_modified;
}

inline string Tree::get_round() const
{
    return m_root->get_property("RO", "");
}

inline const Node& Tree::get_root() const
{
    return *m_root;
}

inline string Tree::get_time() const
{
    return m_root->get_property("TM", "");
}

inline bool Tree::is_doubtful_move(const Node& node) const
{
    return node.has_property("DO");
}

inline bool Tree::is_interesting_move(const Node& node) const
{
    return node.has_property("IT");
}

inline Node& Tree::non_const(const Node& node)
{
    LIBBOARDGAME_ASSERT(contains(node));
    return const_cast<Node&>(node);
}

inline unique_ptr<Node> Tree::remove_children(const Node& node)
{
    if (node.has_children())
        m_modified = true;
    return non_const(node).remove_children();
}

inline void Tree::set_charset(const string& charset)
{
    set_property(get_root(), "CA", charset);
}

inline void Tree::set_date(const string& date)
{
    set_property(get_root(), "DT", date);
}

inline void Tree::set_event(const string& event)
{
    set_property(get_root(), "EV", event);
}

inline void Tree::set_modified()
{
    m_modified = true;
}

template<typename T>
void Tree::set_property(const Node& node, const string& id, const T& value)
{
    bool was_changed = non_const(node).set_property(id, value);
    if (was_changed)
        m_modified = true;
}

template<typename T>
void Tree::set_property(const Node& node, const string& id,
                        const vector<T>& values)
{
    bool was_changed = non_const(node).set_property(id, values);
    if (was_changed)
        m_modified = true;
}

inline void Tree::set_round(const string& round)
{
    set_property(get_root(), "RO", round);
}

inline void Tree::set_time(const string& time)
{
    set_property(get_root(), "TM", time);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_TREE_H
