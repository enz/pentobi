//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/Tree.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_TREE_H
#define LIBBOARDGAME_MCTS_TREE_H

#include <algorithm>
#include <memory>
#include "Node.h"
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
    all nodes, but only as const references.<p>
    The tree uses separate parts of the node storage for different threads,
    so it can be used without locking in multi-threaded search. Not all
    functions are thread-safe, only the ones that are used during a search
    (e.g. expanding a node is thread-safe, but clear() is not) */
template<typename M>
class Tree
{
    friend class NodeExpander;

public:
    typedef M Move;

    typedef libboardgame_mcts::Node<M> Node;

    /** Helper class that is passed to the search state during node expansion.
        This class allows the search state to directly create children of a
        node at the node expansion, so that copying to a temporary move list
        is not necessary, but avoids that the search needs to expose a
        non-const reference to the tree to the state. */
    class NodeExpander
    {
    public:
        NodeExpander(unsigned thread_id, Tree& tree, const Node& node);

        /** Add new child.
            The child will only be added if the tree is not full. */
        void add_child(const Move& mv, Float value, Float count,
                       Float rave_value, Float rave_count);

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
        unsigned m_thread_id;

        bool m_is_tree_full;

        int m_nu_children;

        Float m_best_value;

        Tree& m_tree;

        const Node& m_node;

        const Node* m_first_child;

        const Node* m_best_child;
    };

    Tree(size_t max_nodes, unsigned nu_threads);

    ~Tree() throw();

    /** Remove all nodes and initialize the root node with count 0 and a given
        value. */
    void clear(Float root_value);

    const Node& get_root() const;

    size_t get_nu_nodes() const;

    const Node& get_node(NodeIndex i) const;

    void link_children(const Node& node, const Node* first_child,
                       unsigned nu_children);

    void set_max_nodes(size_t max_nodes);

    size_t get_max_nodes() const;

    bool create_node(unsigned thread_id, const Move& mv, Float value,
                     Float count, Float rave_value, Float rave_count);

    void add_value(const Node& node, Float v);

    void add_rave_value(const Node& node, Float v, Float weight);

    /** Reset the root count to 0 and the value to a given value. */
    void clear_root_value(Float value);

    void swap(Tree& tree);

    /** Extract a subtree.
        This operation can be lengthy and can be aborted by providing an abort
        checker argument. If the extraction was aborted, the copied subtree is
        a valid partial tree of the full tree that would have been extracted.
        Note that you still have to re-initialize the value of the subtree
        after the extraction because the value of the root node and the values
        of inner nodes have a different meaning.
        @pre Target tree is empty (! target.get_root().has_children())
        @param target The target tree
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
                      Float min_count = 0, bool check_abort = false,
                      IntervalChecker* interval_checker = 0) const;

    /** Remove a child.
        This function keeps the order of the remaining children and invalidates
        references to children. the operation is not thread-safe. */
    bool remove_child(const Node& node, const Move& mv);

private:
    struct ThreadStorage
    {
        Node* begin;

        Node* end;

        Node* next;
    };

    unsigned m_nu_threads;

    size_t m_max_nodes;

    size_t m_nodes_per_thread;

    unique_ptr<ThreadStorage[]> m_thread_storage;

    unique_ptr<Node[]> m_nodes;

    bool contains(const Node& node) const;

    unsigned get_thread_storage(const Node& node) const;

    Node& non_const(const Node& node) const;
};

template<typename M>
inline Tree<M>::NodeExpander::NodeExpander(unsigned thread_id, Tree& tree,
                                           const Node& node)
    : m_thread_id(thread_id),
      m_is_tree_full(false),
      m_nu_children(0),
      m_best_value(0),
      m_tree(tree),
      m_node(node),
      m_first_child(m_tree.m_thread_storage[thread_id].next),
      m_best_child(m_first_child)
{
}

template<typename M>
inline void Tree<M>::NodeExpander::add_child(const Move& mv, Float value,
                                             Float count, Float rave_value,
                                             Float rave_count)
{
    LIBBOARDGAME_ASSERT(m_nu_children < numeric_limits<int>::max());
    if (! (m_is_tree_full |= ! m_tree.create_node(m_thread_id, mv, value,
                                                  count, rave_value,
                                                  rave_count)))
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
inline const Node<M>* Tree<M>::NodeExpander::get_best_child() const
{
    if (m_nu_children > 0)
        return m_best_child;
    else
        return 0;
}

template<typename M>
inline const Node<M>& Tree<M>::get_node(NodeIndex i) const
{
    return m_nodes[i];
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
        m_tree.link_children(m_node, m_first_child, m_nu_children);
}

template<typename M>
Tree<M>::Tree(size_t max_nodes, unsigned nu_threads)
    : m_nu_threads(nu_threads)
{
    set_max_nodes(max_nodes);
}

template<typename M>
Tree<M>::~Tree() throw()
{
}

template<typename M>
inline void Tree<M>::add_rave_value(const Node& node, Float v, Float weight)
{
    non_const(node).add_rave_value(v, weight);
}

template<typename M>
inline void Tree<M>::add_value(const Node& node, Float v)
{
    non_const(node).add_value(v);
}

template<typename M>
void Tree<M>::clear(Float root_value)
{
    m_thread_storage[0].next = m_thread_storage[0].begin + 1;
    for (unsigned i = 1; i < m_nu_threads; ++i)
        m_thread_storage[i].next = m_thread_storage[i].begin;
    m_nodes[0].init(Move::null(), root_value, 0, root_value, 0);
}

template<typename M>
inline void Tree<M>::clear_root_value(Float value)
{
    m_nodes[0].init_value(value, 0);
}

template<typename M>
bool Tree<M>::contains(const Node& node) const
{
    return (&node >= m_nodes.get() && &node < m_nodes.get() + m_max_nodes);
}

template<typename M>
bool Tree<M>::copy_subtree(Tree& target, const Node& target_node,
                           const Node& node, Float min_count,
                           bool check_abort,
                           IntervalChecker* interval_checker) const
{
    LIBBOARDGAME_ASSERT(target.m_max_nodes == m_max_nodes);
    LIBBOARDGAME_ASSERT(target.m_nu_threads == m_nu_threads);
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
    unsigned nu_children = node.get_nu_children();
    const Node& first_child = get_node(node.get_first_child());
    // Create target children in the equivalent thread storage as in source.
    // This ensures that the thread storage will not overflow (because the
    // trees have identical nu_threads/max_nodes)
    ThreadStorage& thread_storage =
        target.m_thread_storage[get_thread_storage(first_child)];
    const Node* target_child = thread_storage.next;
    NodeIndex target_first_child =
        static_cast<NodeIndex>(target_child - target.m_nodes.get());
    target_node_non_const.link_children(target_first_child, nu_children);
    thread_storage.next += nu_children;
    // Without the extra () around thread_storage.next in the following
    // assert, GCC 4.6.1 gives the error: parse error in template argument list
    LIBBOARDGAME_ASSERT((thread_storage.next) < thread_storage.end);
    abort = false;
    const Node* end = &first_child + node.get_nu_children();
    for (const Node* i = &first_child; i != end; ++i, ++target_child)
        if (! copy_subtree(target, *target_child, *i, min_count,
                           check_abort, interval_checker))
            // Finish this loop even on abort to make sure the children
            // node data is copied
            abort = true;
    return ! abort;
}

template<typename M>
bool Tree<M>::create_node(unsigned thread_id, const Move& mv,
                          Float value, Float count, Float rave_value,
                          Float rave_count)
{
    LIBBOARDGAME_ASSERT(thread_id < m_nu_threads);
    ThreadStorage& thread_storage = m_thread_storage[thread_id];
    if (thread_storage.next != thread_storage.end)
    {
        thread_storage.next->init(mv, value, count, rave_value, rave_count);
        ++thread_storage.next;
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
    LIBBOARDGAME_ASSERT(! target.get_root().has_children());
    return copy_subtree(target, target.m_nodes[0], node, 0, check_abort,
                        interval_checker);
}

template<typename M>
size_t Tree<M>::get_max_nodes() const
{
    return m_max_nodes;
}

template<typename M>
size_t Tree<M>::get_nu_nodes() const
{
    size_t result = 0;
    for (unsigned i = 0; i < m_nu_threads; ++i)
    {
        ThreadStorage& thread_storage = m_thread_storage[i];
        result += thread_storage.next - thread_storage.begin;
    }
    return result;
}

template<typename M>
inline const typename Tree<M>::Node& Tree<M>::get_root() const
{
    return m_nodes[0];
}

/** Get the thread storage a node belongs to. */
template<typename M>
inline unsigned Tree<M>::get_thread_storage(const Node& node) const
{
    size_t diff = &node - m_nodes.get();
    return static_cast<unsigned>(diff / m_nodes_per_thread);
}

template<typename M>
inline void Tree<M>::link_children(const Node& node, const Node* first_child,
                                   unsigned nu_children)
{
    NodeIndex first_child_index =
        static_cast<NodeIndex>(first_child - m_nodes.get());
    LIBBOARDGAME_ASSERT(first_child_index > 0);
    LIBBOARDGAME_ASSERT(first_child_index < m_max_nodes);
    non_const(node).link_children(first_child_index, nu_children);
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
    unsigned nu_children = node.get_nu_children();
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
    max_nodes =
        min(max_nodes, static_cast<size_t>(numeric_limits<NodeIndex>::max()));
    if (max_nodes == 0)
        // We need at least the root node (for useful searches we need of
        // course also children, but a root node is the minimum requirement to
        // avoid crashing).
        max_nodes = 1;
    m_max_nodes = max_nodes;
    m_nodes.reset(new Node[max_nodes]);
    m_thread_storage.reset(new ThreadStorage[m_nu_threads]);
    m_nodes_per_thread = max_nodes / m_nu_threads;
    for (unsigned i = 0; i < m_nu_threads; ++i)
    {
        ThreadStorage& thread_storage = m_thread_storage[i];
        thread_storage.begin = m_nodes.get() + i * m_nodes_per_thread;
        thread_storage.end = thread_storage.begin + m_nodes_per_thread;
    }
    clear(0);
}

template<typename M>
void Tree<M>::swap(Tree& tree)
{
    // Reminder to update this function when the class gets additional members
    struct Dummy
    {
        unsigned m_nu_threads;
        size_t m_max_nodes;
        size_t m_nodes_per_thread;
        unique_ptr<ThreadStorage> m_thread_storage;
        unique_ptr<Node[]> m_nodes;
    };
    static_assert(sizeof(Tree) == sizeof(Dummy),
                  "libboardgame_mcts::Tree::swap needs updating");
    std::swap(m_nu_threads, tree.m_nu_threads);
    std::swap(m_max_nodes, tree.m_max_nodes);
    std::swap(m_nodes_per_thread, tree.m_nodes_per_thread);
    m_thread_storage.swap(tree.m_thread_storage);
    m_nodes.swap(tree.m_nodes);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_TREE_H
