//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/Node.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_NODE_H
#define LIBBOARDGAME_MCTS_NODE_H

#include <cstdint>
#include "Atomic.h"
#include "libboardgame_util/Assert.h"

namespace libboardgame_mcts {

using namespace std;

//-----------------------------------------------------------------------------

typedef uint_least32_t NodeIdx;

//-----------------------------------------------------------------------------

/** %Node in a MCTS tree.
    For details about how the nodes are used in lock-free multi-threaded mode,
    see @ref libboardgame_doc_enz_2009. */
template<typename M, typename F, bool MT>
class Node
{
public:
    typedef M Move;

    typedef F Float;

    Node() = default;

    Node(const Node&) = delete;

    Node& operator=(const Node&) = delete;

    /** Initialize the node.
        This function may not be called on a node that is already part of
        the tree in multi-threaded mode.
        The node may be initialized with values and counts greater zero
        (prior knowledge) but even if it is initialized with count zero, it
        must be initialized with a usable value (e.g. first play urgency for
        inner nodes or tie value for the root node). */
    void init(const Move& mv, Float value, Float count);

    const Move& get_move() const;

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

    bool has_children() const;

    unsigned short get_nu_children() const;

    /** Set a new value.
        This operation is needed when reusing a subtree from a previous search
        because the value of root nodes and inner nodes have a different
        meaning (position value vs. move values) so the root value cannot be
        reused but all other nodes in the tree can be reused without
        changes.
        It is not thread-safe and may not be called during the search. */
    void init_value(Float value, Float count);

    /** Copy the value count from another node without changing the child
        information.
        This function is not thread-safe and may not be called during the
        search. */
    void copy_data_from(const Node& node);

    void link_children(NodeIdx first_child, unsigned short nu_children);

    void unlink_children();

    void add_value(Float v, Float weight = 1);

    void remove_value(Float v, Float weight = 1);

    void inc_visit_count();

    /** Get node index of first child.
        @pre has_children() */
    NodeIdx get_first_child() const;

private:
    Atomic<Float, MT> m_value;

    Atomic<Float, MT> m_value_count;

    Atomic<Float, MT> m_visit_count;

    Atomic<unsigned short, MT> m_nu_children;

    Move m_move;

    NodeIdx m_first_child;
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
void Node<M, F, MT>::copy_data_from(const Node& node)
{
    // Reminder to update this function when the class gets additional members
    struct Dummy
    {
        Atomic<Float, MT> m_value;
        Atomic<Float, MT> m_value_count;
        Atomic<Float, MT> m_visit_count;
        Atomic<unsigned short, MT> m_nu_children;
        Move m_move;
        NodeIdx m_first_child;
    };
    static_assert(sizeof(Node) == sizeof(Dummy), "");

    m_move = node.m_move;
    m_value_count = node.m_value_count.load();
    m_value = node.m_value.load();
    m_visit_count = node.m_visit_count.load();
}

template<typename M, typename F, bool MT>
inline auto Node<M, F, MT>::get_value_count() const -> Float
{
    return m_value_count.load(memory_order_relaxed);
}

template<typename M, typename F, bool MT>
inline NodeIdx Node<M, F, MT>::get_first_child() const
{
    LIBBOARDGAME_ASSERT(has_children());
    return m_first_child;
}

template<typename M, typename F, bool MT>
inline auto Node<M, F, MT>::get_move() const -> const Move&
{
    return m_move;
}

template<typename M, typename F, bool MT>
inline unsigned short Node<M, F, MT>::get_nu_children() const
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
inline bool Node<M, F, MT>::has_children() const
{
    return get_nu_children() > 0;
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
void Node<M, F, MT>::init(const Move& mv, Float value, Float count)
{
    // The node is not yet visible to other threads because init() is called
    // before the children are linked to its parent with link_children()
    // (which does a memory_order_release on m_nu_children of the parent).
    // Therefore, the most efficient way here is to initialize all values with
    // memory_order_relaxed.
    m_move = mv;
    m_value_count.store(count, memory_order_relaxed);
    m_value.store(value, memory_order_relaxed);
    m_visit_count.store(0, memory_order_relaxed);
    m_nu_children.store(0, memory_order_relaxed);
}

template<typename M, typename F, bool MT>
void Node<M, F, MT>::init_value(Float value, Float count)
{
    m_value_count = count;
    m_value = value;
}

template<typename M, typename F, bool MT>
inline void Node<M, F, MT>::link_children(NodeIdx first_child,
                                          unsigned short nu_children)
{
    LIBBOARDGAME_ASSERT(nu_children < Move::range);
    // first_child cannot be 0 because 0 is always used for the root node
    LIBBOARDGAME_ASSERT(first_child != 0);
    m_first_child = first_child;
    m_nu_children.store(nu_children, memory_order_release);
}

template<typename M, typename F, bool MT>
void Node<M, F, MT>::remove_value(Float v, Float weight)
{
    // Intentionally uses no synchronization and does not care about
    // lost updates in multi-threaded mode
    Float count = m_value_count.load(memory_order_relaxed);
    if (count > weight)
    {
        Float value = m_value.load(memory_order_relaxed);
        count -= weight;
        value -= weight * (v - value) / count;
        m_value.store(value, memory_order_relaxed);
        m_value_count.store(count, memory_order_relaxed);
    }
    else
        m_value_count.store(0, memory_order_relaxed);
}

template<typename M, typename F, bool MT>
inline void Node<M, F, MT>::unlink_children()
{
    m_nu_children.store(0, memory_order_release);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_NODE_H
