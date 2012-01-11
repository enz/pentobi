//-----------------------------------------------------------------------------
/** @file mcts/Tree.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_TREE_H
#define LIBBOARDGAME_MCTS_TREE_H

#include <algorithm>
#include <memory>
#include "ChildIterator.h"
#include "libboardgame_util/Abort.h"
#include "libboardgame_util/IntervalChecker.h"

namespace libboardgame_mcts {

using namespace std;
using libboardgame_util::get_abort;
using libboardgame_util::IntervalChecker;

//-----------------------------------------------------------------------------

/** Tree for Monte-Carlo tree search.
    The nodes can be modified only through member functions of this class,
    so that it can guarantee an intact tree structure. The user has access to
    all nodes, but only as const references. */
template<typename M>
class Tree
{
    friend class NodeExpander;

public:
    typedef M Move;

    typedef libboardgame_mcts::Node<M> Node;

    typedef libboardgame_mcts::ChildIterator<M> ChildIterator;

    /** Helper class that is passed to the search state during node expansion.
        This class allows the search state to directly create children of a
        node at the node expansion, so that copying to a temporary move list
        is not necessary, but avoids that the search needs to expose a
        non-const reference to the tree to the state. */
    class NodeExpander
    {
    public:
        NodeExpander(Tree& tree, const Node& node);

        /** Add new child.
            The child will only be added if the tree is not full. */
        void add_child(const Move& mv);

        /** Add new child with prior knowledge initialization.
            The child will only be added if the tree is not full. */
        void add_child(const Move& mv, ValueType value, ValueType count,
                       ValueType rave_value, ValueType rave_count);

        /** Link the children to the parent node. */
        void link_children();

        /** Return if the tree capacity was reached during an add_child(). */
        bool is_tree_full() const;

        /** Return the node to play after the node expansion.
            This returns the child with the highest value if prior knowledge
            was used, or the first child, or null if no children. This can be
            used for avoiding and extra iteration over the children when
            selecting a child after a node expansion. */
        const Node* get_best_child() const;

    private:
        bool m_is_tree_full;

        int m_nu_children;

        ValueType m_best_value;

        Tree& m_tree;

        const Node& m_node;

        Node* m_first_child;

        const Node* m_best_child;
    };

    Tree(size_t max_nodes);

    ~Tree() throw();

    void clear();

    const Node& get_root() const;

    size_t get_nu_nodes() const;

    void link_children(const Node& node, const Node& first_child,
                       unsigned int nu_children);

    void set_max_nodes(size_t max_nodes);

    size_t get_max_nodes() const;

    bool create_node(const Move& mv);

    bool create_node(const Move& mv, ValueType value, ValueType count,
                     ValueType rave_value, ValueType rave_count);

    void add_value(const Node& node, ValueType v);

    void add_rave_value(const Node& node, ValueType v, ValueType weight);

    void inc_visit_count(const Node& node);

    /** See Node::clear_values(). */
    void clear_values(const Node& node);

    void swap(Tree& tree);

    /** Extract a subtree.
        This operation can be lengthy and can be aborted by providing an abort
        checker argument. If the extraction was aborted, the copied subtree is
        a valid partial tree of the full tree that would have been extracted.
        @param target The target tree (will be cleared before the extraction)
        @param node The root node of the subtree.
        @param check_abort Whether to check util::get_abort()
        @param interval_checker An optional expensive function to check if the
        extraction should be aborted.
        @return @c false if the extraction was aborted. */
    bool extract_subtree(Tree& target, const Node& node,
                         bool check_abort = false,
                         IntervalChecker* interval_checker = 0) const;

    /** Copy a subtree.
        This operation can be lengthy and can be aborted by providing an abort
        checker argument. If the copying was aborted, the copied subtree is
        a valid partial tree of the full tree that would have been copying.
        The caller is responsible that the trees have the same number of
        maximum nodes and that the target tree has room for the subtree.
        @param target The target tree
        @param target_node The target node
        @param node The root node of the subtree.
        @param min_count Don't copy subtrees of nodes below this count
        @param check_abort Whether to check util::get_abort()
        @param interval_checker
        @return @c false if the copying was aborted. */
    bool copy_subtree(Tree& target, const Node& target_node, const Node& node,
                      ValueType min_count = 0, bool check_abort = false,
                      IntervalChecker* interval_checker = 0) const;

    /** Remove a child.
        This function keeps the order of the remaining children and invalidates
        references to children. the operation is not thread-safe. */
    bool remove_child(const Node& node, const Move& mv);

private:
    unique_ptr<Node[]> m_nodes;

    Node* m_nodes_next;

    Node* m_nodes_end;

    bool contains(const Node& node) const;

    Node& non_const(const Node& node) const;
};

template<typename M>
inline Tree<M>::NodeExpander::NodeExpander(Tree& tree, const Node& node)
    : m_is_tree_full(false),
      m_nu_children(0),
      m_best_value(0),
      m_tree(tree),
      m_node(node),
      m_first_child(m_tree.m_nodes_next),
      m_best_child(m_first_child)
{
}

template<typename M>
inline void Tree<M>::NodeExpander::add_child(const Move& mv)
{
    LIBBOARDGAME_ASSERT(m_nu_children < numeric_limits<int>::max());
    if (! (m_is_tree_full |= ! m_tree.create_node(mv)))
        ++m_nu_children;
}

template<typename M>
void Tree<M>::NodeExpander::add_child(const Move& mv, ValueType value,
                                      ValueType count, ValueType rave_value,
                                      ValueType rave_count)
{
    LIBBOARDGAME_ASSERT(m_nu_children < numeric_limits<int>::max());
    if (! (m_is_tree_full |= ! m_tree.create_node(mv, value, count,
                                                  rave_value, rave_count)))
    {
        if (m_nu_children == 0)
            m_best_value = value;
        else if (count > 0 && value > m_best_value)
        {
            m_best_child = m_first_child + m_nu_children;
            m_best_value = value;
        }
        ++m_nu_children;
    }
}

template<typename M>
void Tree<M>::clear_values(const Node& node)
{
    non_const(node).clear_values();
}

template<typename M>
inline const Node<M>* Tree<M>::NodeExpander::get_best_child() const
{
    if (m_nu_children > 0)
        return m_best_child;
    else
        return 0;
}

template<typename M>
inline bool Tree<M>::NodeExpander::is_tree_full() const
{
    return m_is_tree_full;
}

template<typename M>
inline void Tree<M>::NodeExpander::link_children()
{
    if (m_nu_children > 0)
        m_tree.link_children(m_node, *m_first_child, m_nu_children);
}

template<typename M>
Tree<M>::Tree(size_t max_nodes)
{
    set_max_nodes(max_nodes);
}

template<typename M>
Tree<M>::~Tree() throw()
{
}

template<typename M>
inline void Tree<M>::add_rave_value(const Node& node, ValueType v,
                                    ValueType weight)
{
    non_const(node).add_rave_value(v, weight);
}

template<typename M>
inline void Tree<M>::add_value(const Node& node, ValueType v)
{
    non_const(node).add_value(v);
}

template<typename M>
void Tree<M>::clear()
{
    m_nodes_next = m_nodes.get() + 1;
    m_nodes[0].clear();
}

/** Check if allocator contains node.
    This function uses pointer comparisons. Since the result of comparisons for
    pointers to elements in different containers is platform-dependent, it is
    only guaranteed that it returns true, if not node belongs to the tree, but
    not that it returns false for nodes not in the tree. */
template<typename M>
bool Tree<M>::contains(const Node& node) const
{
    return (&node >= m_nodes.get() && &node < m_nodes_end);
}

template<typename M>
bool Tree<M>::copy_subtree(Tree& target, const Node& target_node,
                           const Node& node, ValueType min_count,
                           bool check_abort,
                           IntervalChecker* interval_checker) const
{
    LIBBOARDGAME_ASSERT(target.get_max_nodes() == get_max_nodes());
    LIBBOARDGAME_ASSERT(contains(node));
    Node& target_node_non_const = target.non_const(target_node);
    target_node_non_const.copy_data_from(node);
    bool abort =
        (check_abort && get_abort())
        || (interval_checker != 0 && (*interval_checker)());
    if (! node.has_children() || node.get_count() < min_count || abort)
    {
        target_node_non_const.unlink_children();
        return ! abort;
    }
    Node* target_child = target.m_nodes_next;
    unsigned int nu_children = node.get_nu_children();
    target_node_non_const.link_children(*target_child, nu_children);
    target.m_nodes_next += nu_children;
    // Don't use target.get_max_nodes() in the following assertion because it
    // uses target.m_nodes_end in its implementation and we want to check that
    // target.m_nodes_end is valid. Use get_max_nodes() instead, we already
    // know that the trees have the same maximum size from the precondition of
    // this function
    LIBBOARDGAME_ASSERT(target.m_nodes_next
                    <= target.m_nodes.get() + get_max_nodes());
    abort = false;
    for (ChildIterator i(node); i; ++i, ++target_child)
        if (! copy_subtree(target, *target_child, *i, min_count))
            // Finish this loop even on abort to make sure the children node
            // data is copied
            abort = true;
    return ! abort;
}

template<typename M>
inline bool Tree<M>::create_node(const Move& mv)
{
    if (m_nodes_next != m_nodes_end)
    {
        m_nodes_next->init(mv);
        ++m_nodes_next;
        return true;
    }
    return false;
}

template<typename M>
bool Tree<M>::create_node(const Move& mv, ValueType value, ValueType count,
                          ValueType rave_value, ValueType rave_count)
{
    if (m_nodes_next != m_nodes_end)
    {
        m_nodes_next->init(mv, value, count, rave_value, rave_count);
        ++m_nodes_next;
        return true;
    }
    return false;
}

template<typename M>
bool Tree<M>::extract_subtree(Tree& target, const Node& node,
                              bool check_abort,
                              IntervalChecker* interval_checker) const
{
    LIBBOARDGAME_ASSERT(contains(node));
    LIBBOARDGAME_ASSERT(&target != this);
    LIBBOARDGAME_ASSERT(target.get_max_nodes() == get_max_nodes());
    target.clear();
    return copy_subtree(target, target.m_nodes[0], node, 0, check_abort,
                        interval_checker);
}

template<typename M>
size_t Tree<M>::get_max_nodes() const
{
    return m_nodes_end - m_nodes.get();
}

template<typename M>
inline size_t Tree<M>::get_nu_nodes() const
{
    return m_nodes_next - m_nodes.get();
}

template<typename M>
inline const typename Tree<M>::Node& Tree<M>::get_root() const
{
    return m_nodes[0];
}

template<typename M>
inline void Tree<M>::inc_visit_count(const Node& node)
{
    non_const(node).inc_visit_count();
}

template<typename M>
inline void Tree<M>::link_children(const Node& node, const Node& first_child,
                                   unsigned int nu_children)
{
    non_const(node).link_children(non_const(first_child), nu_children);
}

/** Convert a const reference to node from user to a non-const reference.
    The user has only read access to the nodes, because the tree guarantees
    the validity of the tree structure. */
template<typename M>
inline typename Tree<M>::Node& Tree<M>::non_const(const Node& node) const
{
    LIBBOARDGAME_ASSERT(contains(node));
    return const_cast<Node&>(node);
}

template<typename M>
bool Tree<M>::remove_child(const Node& node, const Move& mv)
{
    unsigned int nu_children = node.get_nu_children();
    if (nu_children == 0)
        return false;
    Node& first_child = non_const(*node.get_first_child());
    Node* child = &first_child;
    for (int i = 0; i < nu_children; ++i, ++child)
        if (child->get_move() == mv)
        {
            for ( ; i < nu_children - 1; ++i, ++child)
                *child = *(child + 1);
            non_const(node).link_children(first_child, nu_children - 1);
            return true;
        }
    return false;
}

template<typename M>
void Tree<M>::set_max_nodes(size_t max_nodes)
{
    LIBBOARDGAME_ASSERT(max_nodes > 0);
    m_nodes.reset(new Node[max_nodes]);
    m_nodes_end = m_nodes.get() + max_nodes;
    clear();
}

template<typename M>
void Tree<M>::swap(Tree& tree)
{
    // Reminder to update this function when the class gets additional members
    struct Dummy
    {
        unique_ptr<Node[]> m_nodes;
        Node* m_nodes_next;
        Node* m_nodes_end;
    };
    static_assert(sizeof(Tree) == sizeof(Dummy),
                  "libboardgame_mcts::Tree::swap needs updating");

    m_nodes.swap(tree.m_nodes);
    std::swap(m_nodes_next, tree.m_nodes_next);
    std::swap(m_nodes_end, tree.m_nodes_end);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_TREE_H
