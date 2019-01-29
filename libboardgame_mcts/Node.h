//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/Node.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_NODE_H
#define LIBBOARDGAME_MCTS_NODE_H

#include <limits>
#include "Atomic.h"
#include "libboardgame_util/Assert.h"

namespace libboardgame_mcts {

using namespace std;

//-----------------------------------------------------------------------------

using NodeIdx = uint_least32_t;

//-----------------------------------------------------------------------------

/** %Node in a MCTS tree.
    For details about how the nodes are used in lock-free multi-threaded mode,
    see @ref libboardgame_doc_enz_2009. */
template<typename M, typename F, bool MT>
class Node
{
public:
    using Move = M;

    using Float = F;

    /** Value returned by get_nu_children() if node has not been expanded. */
    static constexpr short value_unexpanded = -2;

    /** Value returned by get_nu_children() if node is currently expanding. */
    static constexpr short value_expanding = -1;

    static constexpr unsigned max_children = numeric_limits<short>::max();

    Node() = default;

    Node(const Node&) = delete;

    Node& operator=(const Node&) = delete;

    /** Initialize the node.
        This function may not be called on a node that is already part of
        the tree in multi-threaded mode. */
    void init(const Move& mv, Float value, Float count, Float move_prior);

    /** Initializes the root node.
        Does not initialize value and value count as they are not used for the
        root. */
    void init_root();

    const Move& get_move() const { return m_move; }

    /** Prior value for the move.
        This value is used in the exploration term, see description of class
        SearchBase. */
    Float get_move_prior() const { return m_move_prior; }

    /** Number of simulations that went through this node. */
    Float get_visit_count() const;

    /** Number of values that were added.
        This count is usually larger than the visit count because in addition
        to the terminal values of the simulations, prior knowledge values and
        weighted RAVE values could have been added added. */
    Float get_value_count() const;

    /** Value of the node.
        For the root node, this is the value of the position from the point of
        view of the player at the root node; for all other nodes, this is the
        value of the move leading to the position at the node from the point
        of view of the player at the parent node. */
    Float get_value() const;

    bool is_unexpanded() const { return get_nu_children() == value_unexpanded; }

    void set_expanding();

    /** Get the number of children or the expansion state.
        @return The number of children, if the node has been expanded (0 means
        terminal game state), otherwise the negative values value_unexpanded
        or value_expanding. */
    short get_nu_children() const;

    /** Copy the value count from another node without changing the child
        information.
        This function is not thread-safe and may not be called during the
        search. */
    void copy_data_from(const Node& node);

    void link_children(NodeIdx first_child, unsigned nu_children);

    /** Faster version of link_children() for single-threaded parts of the
        code. */
    void link_children_st(NodeIdx first_child, unsigned nu_children);

    /** Unlink children.
        Only to be used in single-threaded parts of the code. */
    void unlink_children_st();

    void add_value(Float v, Float weight = 1);

    /** Add a value with weight 1 and remove a previously added loss.
        Needed for the implementation of virtual losses in multi-threaded
        MCTS and more efficient that a separate add and remove call. */
    void add_value_remove_loss(Float v);

    void inc_visit_count();

    /** Get node index of first child.
        @pre get_nu_children() > 0. Note that in lock-free search, it can
        happen that get_nu_children() was greater 0 but becomes negative
        again if two threads expand the node simultaneosly and one thread
        sets nu_children to value_expanding because it missed that the other
        thread already expanded it. But since nodes never get deleted during
        the lock-free search, and the number of children is deterministic,
        any value greater 0 returned by get_nu_children() will be in a
        consistent state with the children pointed to by get_first_child(). */
    NodeIdx get_first_child() const;

private:
    Atomic<Float, MT> m_value;

    Atomic<Float, MT> m_value_count;

    Atomic<Float, MT> m_visit_count;

    Float m_move_prior;

    /** See get_nu_children() */
    Atomic<short, MT> m_nu_children;

    Move m_move;

    Atomic<NodeIdx, MT> m_first_child;
};

template<typename M, typename F, bool MT>
void Node<M, F, MT>::add_value(Float v, Float weight)
{
    // Intentionally uses no synchronization and does not care about
    // lost updates in multi-threaded mode
    Float count = m_value_count.load(memory_order_relaxed);
    Float value = m_value.load(memory_order_relaxed);
    count += weight;
    value += weight * (v - value) / count;
    m_value.store(value, memory_order_relaxed);
    m_value_count.store(count, memory_order_relaxed);
}

template<typename M, typename F, bool MT>
void Node<M, F, MT>::add_value_remove_loss(Float v)
{
    // Intentionally uses no synchronization and does not care about
    // lost updates in multi-threaded mode
    Float count = m_value_count.load(memory_order_relaxed);
    if (count == 0)
        return; // Adding the virtual loss was a lost update
    Float value = m_value.load(memory_order_relaxed);
    value += v / count;
    m_value.store(value, memory_order_relaxed);
}

template<typename M, typename F, bool MT>
void Node<M, F, MT>::copy_data_from(const Node& node)
{
    // Reminder to update this function when the class gets additional members
    struct Dummy
    {
        Atomic<Float, MT> m_value;
        Atomic<Float, MT> m_value_count;
        Atomic<Float, MT> m_visit_count;
        Float m_move_prior;
        Atomic<short, MT> m_nu_children;
        Move m_move;
        NodeIdx m_first_child;
    };
    static_assert(sizeof(Node) == sizeof(Dummy));

    m_move = node.m_move;
    m_move_prior = node.m_move_prior;
    // Load/store relaxed (it wouldn't even need to be atomic) because this
    // function is only used before the multi-threaded search.
    m_value_count.store(node.m_value_count.load(memory_order_relaxed),
                        memory_order_relaxed);
    m_value.store(node.m_value.load(memory_order_relaxed),
                  memory_order_relaxed);
    m_visit_count.store(node.m_visit_count.load(memory_order_relaxed),
                        memory_order_relaxed);
}

template<typename M, typename F, bool MT>
inline auto Node<M, F, MT>::get_value_count() const -> Float
{
    return m_value_count.load(memory_order_relaxed);
}

template<typename M, typename F, bool MT>
inline NodeIdx Node<M, F, MT>::get_first_child() const
{
    return m_first_child.load(memory_order_acquire);
}

template<typename M, typename F, bool MT>
inline short Node<M, F, MT>::get_nu_children() const
{
    return m_nu_children.load(memory_order_acquire);
}

template<typename M, typename F, bool MT>
inline auto Node<M, F, MT>::get_value() const -> Float
{
    return m_value.load(memory_order_relaxed);
}

template<typename M, typename F, bool MT>
inline auto Node<M, F, MT>::get_visit_count() const -> Float
{
    return m_visit_count.load(memory_order_relaxed);
}

template<typename M, typename F, bool MT>
inline void Node<M, F, MT>::inc_visit_count()
{
    // We don't care about the unlikely case that updates are lost because
    // incrementing is not atomic
    Float count = m_visit_count.load(memory_order_relaxed);
    ++count;
    m_visit_count.store(count, memory_order_relaxed);
}

template<typename M, typename F, bool MT>
void Node<M, F, MT>::init(const Move& mv, Float value, Float count,
                          Float move_prior)
{
    // The node is not yet visible to other threads because init() is called
    // before the children are linked to its parent with link_children()
    // (which does a memory_order_release on m_nu_children of the parent).
    // Therefore, the most efficient way here is to initialize all values with
    // memory_order_relaxed.
    m_move = mv;
    m_move_prior = move_prior;
    m_value_count.store(count, memory_order_relaxed);
    m_value.store(value, memory_order_relaxed);
    m_visit_count.store(0, memory_order_relaxed);
    m_nu_children.store(value_unexpanded, memory_order_relaxed);
}

template<typename M, typename F, bool MT>
void Node<M, F, MT>::init_root()
{
#ifdef LIBBOARDGAME_DEBUG
    m_move = Move::null();
#endif
    m_visit_count.store(0, memory_order_relaxed);
    m_nu_children.store(value_unexpanded, memory_order_relaxed);
}

template<typename M, typename F, bool MT>
inline void Node<M, F, MT>::link_children(NodeIdx first_child,
                                          unsigned nu_children)
{
    LIBBOARDGAME_ASSERT(nu_children < max_children);
    LIBBOARDGAME_ASSERT(nu_children < Move::range);
    // first_child cannot be 0 because 0 is always used for the root node
    LIBBOARDGAME_ASSERT(first_child != 0);
    // Note that we need release/acquire order for both m_nu_children and
    // m_first_child because because the lock-free search cannot guarantee that
    // a node is not expanded by two threads simultaneously (even if it tries
    // to minimize this probability). Therefore it can happen that
    // m_nu_children had already been set to greater 0 by the first thread and
    // then the children of the second thread must be visible to all threads as
    // soon as the second thread overwrites m_first_child.
    m_first_child.store(first_child, memory_order_release);
    m_nu_children.store(static_cast<short>(nu_children), memory_order_release);
}

template<typename M, typename F, bool MT>
inline void Node<M, F, MT>::link_children_st(NodeIdx first_child,
                                             unsigned nu_children)
{
    LIBBOARDGAME_ASSERT(nu_children < max_children);
    LIBBOARDGAME_ASSERT(nu_children < Move::range);
    // first_child cannot be 0 because 0 is always used for the root node
    LIBBOARDGAME_ASSERT(first_child != 0);
    // Store relaxed (wouldn't even need to be atomic)
    m_first_child.store(first_child, memory_order_relaxed);
    m_nu_children.store(static_cast<short>(nu_children), memory_order_relaxed);
}

template<typename M, typename F, bool MT>
void Node<M, F, MT>::set_expanding()
{
    m_nu_children.store(value_expanding, memory_order_relaxed);
}

template<typename M, typename F, bool MT>
inline void Node<M, F, MT>::unlink_children_st()
{
    // Store relaxed (wouldn't even need to be atomic)
    m_nu_children.store(value_unexpanded, memory_order_relaxed);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_NODE_H
