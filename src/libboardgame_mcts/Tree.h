//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/Tree.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
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

/** %Tree for Monte-Carlo tree search.
    The nodes can be modified only through member functions of this class,
    so that it can guarantee an intact tree structure. The user has access to
    all nodes, but only as const references.<p>
    The tree uses separate parts of the node storage for different threads,
    so it can be used without locking in multi-threaded search. Not all
    functions are thread-safe, only the ones that are used during a search
    (e.g. expanding a node is thread-safe, but clear() is not) */
template<typename N>
class Tree
{
    struct ThreadStorage;

    friend class NodeExpander;

public:
    typedef N Node;

    typedef typename Node::Move Move;

    typedef typename Node::Float Float;

    /** Range for iterating over the children of a node. */
    class Children
    {
    public:
        Children(const Tree& tree, const Node& node)
        {
            auto nu_children = node.get_nu_children();
            m_begin = (nu_children != 0 ?
                        &tree.get_node(node.get_first_child()) : nullptr);
            m_end = m_begin + nu_children;
        }

        const Node* begin() const
        {
            return m_begin;
        }

        const Node* end() const
        {
            return m_end;
        }

        bool empty() const
        {
            return m_begin == nullptr;
        }

    private:
        const Node* m_begin;

        const Node* m_end;
    };


    /** Helper class that is passed to the search state during node expansion.
        This class allows the search state to directly create children of a
        node at the node expansion, so that copying to a temporary move list
        is not necessary, but avoids that the search needs to expose a
        non-const reference to the tree to the state. */
    class NodeExpander
    {
    public:
        /** Constructor.
            @param thread_id
            @param tree
            @param child_min_count The minimum count used for initializing
            children. Used only in debug mode to assert that the children
            are really initialized with a minimum count as declared with
            SearchParamConst::child_min_count. */
        NodeExpander(unsigned thread_id, Tree& tree, Float child_min_count);

        /** Add new child.
            The child will only be added if the tree is not full. */
        void add_child(const Move& mv, Float value, Float count);

        /** Link the children to the parent node. */
        void link_children(Tree& tree, const Node& node);

        /** Return if the tree capacity was reached during an add_child(). */
        bool is_tree_full() const;

        /** Return the node to play after the node expansion.
            This returns the child with the highest value if prior knowledge
            was used, or the first child, or null if no children. This can be
            used for avoiding and extra iteration over the children when
            selecting a child after a node expansion. */
        const Node* get_best_child() const;

    private:
        ThreadStorage& m_thread_storage;

        unsigned short m_nu_children = 0;

        Float m_best_value = 0;

        const Node* m_first_child;

        const Node* m_best_child;

#if LIBBOARDGAME_DEBUG
        Float m_child_min_count;
#endif
    };

    Tree(size_t max_nodes, unsigned nu_threads);

    ~Tree();

    /** Remove all nodes and initialize the root node with count 0 and a given
        value. */
    void clear(Float root_value);

    const Node& get_root() const;

    Children get_children(const Node& node) const
    {
        return Children(*this, node);
    }

    Children get_root_children() const
    {
        return get_children(get_root());
    }

    size_t get_nu_nodes() const;

    const Node& get_node(NodeIdx i) const;

    void link_children(const Node& node, const Node* first_child,
                       unsigned short nu_children);

    size_t get_max_nodes() const;

    void add_value(const Node& node, Float v);

    void add_value(const Node& node, Float v, Float weight);

    void add_value_remove_loss(const Node& node, Float v);

    void inc_visit_count(const Node& node);

    /** Overwrite the root value and count. */
    void init_root_value(Float value, Float count);

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
                         IntervalChecker* interval_checker = nullptr) const;

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
                      IntervalChecker* interval_checker = nullptr) const;

private:
    struct ThreadStorage
    {
        Node* begin;

        Node* end;

        Node* next;
    };

    unique_ptr<Node[]> m_nodes;

    unique_ptr<ThreadStorage[]> m_thread_storage;

    unsigned m_nu_threads;

    size_t m_max_nodes;

    size_t m_nodes_per_thread;

    bool contains(const Node& node) const;

    unsigned get_thread_storage(const Node& node) const;

    Node& non_const(const Node& node) const;
};

template<typename N>
inline Tree<N>::NodeExpander::NodeExpander(unsigned thread_id, Tree& tree,
                                           Float child_min_count)
    : m_thread_storage(tree.m_thread_storage[thread_id]),
      m_first_child(m_thread_storage.next),
      m_best_child(m_first_child)
{
    LIBBOARDGAME_ASSERT(thread_id < tree.m_nu_threads);
#if LIBBOARDGAME_DEBUG
    m_child_min_count = child_min_count;
#else
    LIBBOARDGAME_UNUSED(child_min_count);
#endif
}

template<typename N>
inline void Tree<N>::NodeExpander::add_child(const Move& mv, Float value,
                                             Float count)
{
    LIBBOARDGAME_ASSERT(count >= m_child_min_count);
    if (m_thread_storage.next >= m_thread_storage.end)
        return;
    m_thread_storage.next->init(mv, value, count);
    ++m_thread_storage.next;
    if (m_nu_children == 0)
        m_best_value = value;
    else if (value > m_best_value)
    {
        m_best_child = m_first_child + m_nu_children;
        m_best_value = value;
    }
    ++m_nu_children;
}

template<typename N>
inline auto Tree<N>::NodeExpander::get_best_child() const -> const Node*
{
    if (m_nu_children > 0)
        return m_best_child;
    else
        return nullptr;
}

template<typename N>
inline auto Tree<N>::get_node(NodeIdx i) const -> const Node&
{
    return m_nodes[i];
}

template<typename N>
inline bool Tree<N>::NodeExpander::is_tree_full() const
{
    return m_thread_storage.next >= m_thread_storage.end;
}

template<typename N>
inline void Tree<N>::NodeExpander::link_children(Tree& tree, const Node& node)
{
    if (m_nu_children > 0)
        tree.link_children(node, m_first_child, m_nu_children);
}


template<typename N>
Tree<N>::Tree(size_t max_nodes, unsigned nu_threads)
    : m_nu_threads(nu_threads)
{
    // It doesn't make sense to set max_nodes higher than what can be accessed
    // with NodeIdx
    max_nodes =
        min(max_nodes, static_cast<size_t>(numeric_limits<NodeIdx>::max()));
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
        auto& thread_storage = m_thread_storage[i];
        thread_storage.begin = m_nodes.get() + i * m_nodes_per_thread;
        thread_storage.end = thread_storage.begin + m_nodes_per_thread;
    }
    clear(0);
}

template<typename N>
Tree<N>::~Tree()
{ }

template<typename N>
inline void Tree<N>::add_value(const Node& node, Float v)
{
    non_const(node).add_value(v);
}

template<typename N>
inline void Tree<N>::add_value(const Node& node, Float v, Float weight)
{
    non_const(node).add_value(v, weight);
}

template<typename N>
void Tree<N>::clear(Float root_value)
{
    m_thread_storage[0].next = m_thread_storage[0].begin + 1;
    for (unsigned i = 1; i < m_nu_threads; ++i)
        m_thread_storage[i].next = m_thread_storage[i].begin;
    m_nodes[0].init(Move::null(), root_value, 0);
}

template<typename N>
bool Tree<N>::contains(const Node& node) const
{
    return &node >= m_nodes.get() && &node < m_nodes.get() + m_max_nodes;
}

template<typename N>
bool Tree<N>::copy_subtree(Tree& target, const Node& target_node,
                           const Node& node, Float min_count,
                           bool check_abort,
                           IntervalChecker* interval_checker) const
{
    LIBBOARDGAME_ASSERT(target.m_max_nodes == m_max_nodes);
    LIBBOARDGAME_ASSERT(target.m_nu_threads == m_nu_threads);
    LIBBOARDGAME_ASSERT(contains(node));
    auto& target_node_non_const = target.non_const(target_node);
    target_node_non_const.copy_data_from(node);
    bool abort =
        (check_abort && get_abort())
        || (interval_checker && (*interval_checker)());
    if (! node.has_children()
            || (node.get_visit_count() < min_count && &node != &get_root())
            || abort)
    {
        target_node_non_const.unlink_children();
        return ! abort;
    }
    auto nu_children = node.get_nu_children();
    auto& first_child = get_node(node.get_first_child());
    // Create target children in the equivalent thread storage as in source.
    // This ensures that the thread storage will not overflow (because the
    // trees have identical nu_threads/max_nodes)
    ThreadStorage& thread_storage =
        target.m_thread_storage[get_thread_storage(first_child)];
    auto target_child = thread_storage.next;
    auto target_first_child =
        static_cast<NodeIdx>(target_child - target.m_nodes.get());
    target_node_non_const.link_children(target_first_child, nu_children);
    thread_storage.next += nu_children;
    // Without the extra () around thread_storage.next in the following
    // assert, GCC 4.7.2 gives the error: parse error in template argument list
    LIBBOARDGAME_ASSERT((thread_storage.next) < thread_storage.end);
    abort = false;
    auto end = &first_child + node.get_nu_children();
    for (auto i = &first_child; i != end; ++i, ++target_child)
        if (! copy_subtree(target, *target_child, *i, min_count,
                           check_abort, interval_checker))
            // Finish this loop even on abort to make sure the children
            // node data is copied
            abort = true;
    return ! abort;
}

template<typename N>
bool Tree<N>::extract_subtree(Tree& target, const Node& node,
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

template<typename N>
size_t Tree<N>::get_max_nodes() const
{
    return m_max_nodes;
}

template<typename N>
size_t Tree<N>::get_nu_nodes() const
{
    size_t result = 0;
    for (unsigned i = 0; i < m_nu_threads; ++i)
    {
        auto& thread_storage = m_thread_storage[i];
        result += thread_storage.next - thread_storage.begin;
    }
    return result;
}

template<typename N>
inline auto Tree<N>::get_root() const -> const Node&
{
    return m_nodes[0];
}

/** Get the thread storage a node belongs to. */
template<typename N>
inline unsigned Tree<N>::get_thread_storage(const Node& node) const
{
    size_t diff = &node - m_nodes.get();
    return static_cast<unsigned>(diff / m_nodes_per_thread);
}

template<typename N>
inline void Tree<N>::inc_visit_count(const Node& node)
{
    non_const(node).inc_visit_count();
}

template<typename N>
inline void Tree<N>::init_root_value(Float value, Float count)
{
    m_nodes[0].init_value(value, count);
}

template<typename N>
inline void Tree<N>::link_children(const Node& node, const Node* first_child,
                                   unsigned short nu_children)
{
    NodeIdx first_child_idx = static_cast<NodeIdx>(first_child - m_nodes.get());
    LIBBOARDGAME_ASSERT(first_child_idx > 0);
    LIBBOARDGAME_ASSERT(first_child_idx < m_max_nodes);
    non_const(node).link_children(first_child_idx, nu_children);
}

/** Convert a const reference to node from user to a non-const reference.
    The user has only read access to the nodes, because the tree guarantees
    the validity of the tree structure. */
template<typename N>
inline auto Tree<N>::non_const(const Node& node) const -> Node&
{
    LIBBOARDGAME_ASSERT(contains(node));
    return const_cast<Node&>(node);
}

template<typename N>
inline void Tree<N>::add_value_remove_loss(const Node& node, Float v)
{
    non_const(node).add_value_remove_loss(v);
}

template<typename N>
void Tree<N>::swap(Tree& tree)
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
    static_assert(sizeof(Tree) == sizeof(Dummy), "");
    std::swap(m_nu_threads, tree.m_nu_threads);
    std::swap(m_max_nodes, tree.m_max_nodes);
    std::swap(m_nodes_per_thread, tree.m_nodes_per_thread);
    m_thread_storage.swap(tree.m_thread_storage);
    m_nodes.swap(tree.m_nodes);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_TREE_H
