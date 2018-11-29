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

namespace libboardgame_mcts {

using namespace std;
using libboardgame_util::Range;

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
    using Node = N;

    using Move = typename Node::Move;

    using Float = typename Node::Float;

    /** Range for iterating over the children of a node. */
    using Children = Range<const Node>;


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
            children. Used only in debug mode to verify the arguments for
            add_child().
            @param max_move_prior The maximum move prior used for initializing
            children. Used only in debug mode to verify the arguments for
            add_child(). */
        NodeExpander(unsigned thread_id, Tree& tree, Float child_min_count,
                     Float max_move_prior);

        /** Check if the tree still has the capacity for a given number
            of children. */
        bool check_capacity(unsigned short nu_children) const;

        /** Add new child.
            It needs to be checked first with check_capacity() that the tree
            has enough capacity. */
        void add_child(const Move& mv, Float value, Float count,
                       Float move_prior);

        /** Link the children to the parent node. */
        void link_children(Tree& tree, const Node& node);

        /** Return the node to play after the node expansion.
            This returns the child with the highest value if prior knowledge
            was used, or the first child, or null if no children. This can be
            used for avoiding and extra iteration over the children when
            selecting a child after a node expansion. */
        const Node* get_best_child() const;

    private:
        ThreadStorage& m_thread_storage;

        Float m_best_move_prior = -numeric_limits<Float>::max();

        const Node* m_first_child;

        const Node* m_best_child;

#ifdef LIBBOARDGAME_DEBUG
        Float m_child_min_count;

        Float m_max_move_prior;
#endif
    };

    Tree(size_t memory, unsigned nu_threads);


    /** Remove all nodes but the root node. */
    void clear();

    const Node& get_root() const;

    Children get_children(const Node& node) const;

    Children get_children_nonempty(const Node& node) const;

    Children get_root_children() const { return get_children(get_root()); }

    size_t get_nu_nodes() const;

    const Node& get_node(NodeIdx i) const;

    void link_children(const Node& node, const Node* first_child,
                       unsigned short nu_children);

    void add_value(const Node& node, Float v);

    void add_value(const Node& node, Float v, Float weight);

    void add_value_remove_loss(const Node& node, Float v);

    void inc_visit_count(const Node& node);

    void swap(Tree& tree);

    /** Extract a subtree.
        Note that you still have to re-initialize the value of the subtree
        after the extraction because the value of the root node and the values
        of inner nodes have a different meaning.
        @pre Target tree is empty (! target.get_root().has_children())
        @param target The target tree
        @param node The root node of the subtree. */
    void extract_subtree(Tree& target, const Node& node) const;

    /** Copy a subtree.
        The caller is responsible that the trees have the same number of
        maximum nodes and that the target tree has room for the subtree.
        @param target The target tree
        @param target_node The target node
        @param node The root node of the subtree.
        @param min_count Don't copy subtrees of nodes below this count */
    void copy_subtree(Tree& target, const Node& target_node, const Node& node,
                      Float min_count) const;

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

    void copy_recurse(Tree& target, const Node& target_node, const Node& node,
                      Float min_count) const;

    unsigned get_thread_storage(const Node& node) const;

    Node& non_const(const Node& node) const;
};

template<typename N>
inline Tree<N>::NodeExpander::NodeExpander(
        unsigned thread_id, Tree& tree, [[maybe_unused]] Float child_min_count,
        [[maybe_unused]] Float max_move_prior)
    : m_thread_storage(tree.m_thread_storage[thread_id]),
      m_first_child(m_thread_storage.next),
      m_best_child(nullptr)
{
    LIBBOARDGAME_ASSERT(thread_id < tree.m_nu_threads);
#ifdef LIBBOARDGAME_DEBUG
    m_child_min_count = child_min_count;
    m_max_move_prior = max_move_prior;
#endif
}

template<typename N>
inline void Tree<N>::NodeExpander::add_child(const Move& mv, Float value,
                                             Float count, Float move_prior)
{
    // -numeric_limits<Float>::max() ist init value for m_best_value
    LIBBOARDGAME_ASSERT(value > -numeric_limits<Float>::max());
    LIBBOARDGAME_ASSERT(count >= m_child_min_count);
    LIBBOARDGAME_ASSERT(move_prior <= m_max_move_prior);
    auto& next = m_thread_storage.next;
    LIBBOARDGAME_ASSERT(next < m_thread_storage.end);
    next->init(mv, value, count, move_prior);
    if (move_prior > m_best_move_prior)
    {
        m_best_child = next;
        m_best_move_prior = move_prior;
    }
    ++next;
}

template<typename N>
inline bool Tree<N>::NodeExpander::check_capacity(
        unsigned short nu_children) const
{
    return m_thread_storage.end - m_thread_storage.next  >= nu_children;
}

template<typename N>
inline auto Tree<N>::NodeExpander::get_best_child() const -> const Node*
{
    return m_best_child;
}

template<typename N>
inline auto Tree<N>::get_children(const Node& node) const -> Children
{
    auto nu_children = node.get_nu_children();
    auto begin = nu_children != 0 ? &get_node(node.get_first_child()) : nullptr;
    auto end = begin + nu_children;
    return Children(begin, end);
}

template<typename N>
inline auto Tree<N>::get_children_nonempty(const Node& node) const -> Children
{
    auto begin = &get_node(node.get_first_child());
    auto end = begin + node.get_nu_children();
    return Children(begin, end);
}

template<typename N>
inline auto Tree<N>::get_node(NodeIdx i) const -> const Node&
{
    return m_nodes[i];
}

template<typename N>
inline void Tree<N>::NodeExpander::link_children(Tree& tree, const Node& node)
{
    auto nu_children =
            static_cast<unsigned short>(m_thread_storage.next - m_first_child);
    tree.link_children(node, m_first_child, nu_children);
}


template<typename N>
Tree<N>::Tree(size_t memory, unsigned nu_threads)
{
    if (nu_threads == 0)
        nu_threads = 1;
    auto max_nodes = memory / sizeof(Node);
    // We need at least one node per thread
    max_nodes = max(max_nodes, static_cast<size_t>(nu_threads));
    // It doesn't make sense to set max_nodes higher than what can be accessed
    // with NodeIdx
    max_nodes =
        min(max_nodes, static_cast<size_t>(numeric_limits<NodeIdx>::max()));
    m_nu_threads = nu_threads;
    m_max_nodes = max_nodes;

    // Using make_unique<Node[]>(max_nodes) slows down the array creation and
    // thereby the startup time of Pentobi with GCC 7/8 because the compiler
    // does not optimize away the call to the empty Move() constructor (last
    // tested with GCC 7.2.0 and GCC 8.0.0 on Ubuntu 17.10).
    m_nodes.reset(new Node[max_nodes]);

    m_thread_storage = make_unique<ThreadStorage[]>(nu_threads);
    m_nodes_per_thread = max_nodes / nu_threads;
    for (unsigned i = 0; i < nu_threads; ++i)
    {
        auto& thread_storage = m_thread_storage[i];
        thread_storage.begin = m_nodes.get() + i * m_nodes_per_thread;
        thread_storage.end = thread_storage.begin + m_nodes_per_thread;
    }
    clear();
}

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
void Tree<N>::clear()
{
    m_thread_storage[0].next = m_thread_storage[0].begin + 1;
    for (unsigned i = 1; i < m_nu_threads; ++i)
        m_thread_storage[i].next = m_thread_storage[i].begin;
    m_nodes[0].init_root();
}

template<typename N>
bool Tree<N>::contains(const Node& node) const
{
    return &node >= m_nodes.get() && &node < m_nodes.get() + m_max_nodes;
}

template<typename N>
void Tree<N>::copy_subtree(Tree& target, const Node& target_node,
                           const Node& node, Float min_count) const
{
    target.non_const(target_node).copy_data_from(node);
    if (node.has_children())
        copy_recurse(target, target_node, node, min_count);
    else
        target.non_const(target_node).unlink_children_st();
}

template<typename N>
void Tree<N>::copy_recurse(Tree& target, const Node& target_node,
                           const Node& node, Float min_count) const
{
    LIBBOARDGAME_ASSERT(target.m_max_nodes == m_max_nodes);
    LIBBOARDGAME_ASSERT(target.m_nu_threads == m_nu_threads);
    LIBBOARDGAME_ASSERT(contains(node));
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
    target.non_const(target_node).link_children_st(target_first_child,
                                                   nu_children);
    thread_storage.next += nu_children;
    // Parenthesis around thread_storage.next are needed because of a bug
    // with GCC 4 ("parse error in template argument list")
    LIBBOARDGAME_ASSERT((thread_storage.next) < thread_storage.end);
    auto end = &first_child + node.get_nu_children();
    for (auto i = &first_child; i != end; ++i, ++target_child)
    {
        target_child->copy_data_from(*i);
        if (! i->has_children() || i->get_visit_count() < min_count)
        {
            target_child->unlink_children_st();
            continue;
        }
        copy_recurse(target, *target_child, *i, min_count);
    }
}

template<typename N>
void Tree<N>::extract_subtree(Tree& target, const Node& node) const
{
    LIBBOARDGAME_ASSERT(contains(node));
    LIBBOARDGAME_ASSERT(&target != this);
    LIBBOARDGAME_ASSERT(target.m_max_nodes == m_max_nodes);
    LIBBOARDGAME_ASSERT(! target.get_root().has_children());
    copy_subtree(target, target.m_nodes[0], node, 0);
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
inline void Tree<N>::link_children(const Node& node, const Node* first_child,
                                   unsigned short nu_children)
{
    auto first_child_idx = static_cast<NodeIdx>(first_child - m_nodes.get());
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
    static_assert(sizeof(Tree) == sizeof(Dummy));
    std::swap(m_nu_threads, tree.m_nu_threads);
    std::swap(m_max_nodes, tree.m_max_nodes);
    std::swap(m_nodes_per_thread, tree.m_nodes_per_thread);
    m_thread_storage.swap(tree.m_thread_storage);
    m_nodes.swap(tree.m_nodes);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_TREE_H
