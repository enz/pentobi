//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/Node.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_NODE_H
#define LIBBOARDGAME_MCTS_NODE_H

#include <atomic>
#include <limits>
#include <cstdint>
#include "Float.h"
#include "libboardgame_util/Assert.h"

namespace libboardgame_mcts {

using namespace std;

//-----------------------------------------------------------------------------

typedef uint_least32_t NodeIndex;

//-----------------------------------------------------------------------------

/** Node in a MCTS tree.
    For details about how the nodes are used in lock-free multi-threaded mode,
    see @ref libboardgame_doc_enz_2009. */
template<typename M>
class Node
{
public:
    typedef M Move;

    Node();

    /** Initialize the node.
        This function may not be called on a node that is already part of
        the tree in multi-threaded mode.
        The node may be initialized with values and counts greater zero
        (prior knowledge) but even if it is initialized with count zero, it
        must be initialized with a usable value (e.g. first play urgency for
        inner nodes or tie value for the root node). */
    void init(const Move& mv, Float value, Float count, Float rave_value,
              Float rave_count);

    const Move& get_move() const;

    /** Number of values that were added. */
    Float get_count() const;

    /** Value of the node.
        For the root node, this is the value of the position from the point of
        view of the player at the root node; for all other nodes, this is the
        value of the move leading to the position at the node from the point
        of view of the player at the parent node. */
    Float get_value() const;

    Float get_rave_count() const;

    /** RAVE value of the node.
        For the root node, this is the value id undefined; for all other nodes,
        this is the RAVE value of the move leading to the position at the node
        from the point of view of the player at the parent node.
        See @ref libboardgame_doc_rave. */
    Float get_rave_value() const;

    bool has_children() const;

    unsigned get_nu_children() const;

    /** Set a new value.
        This operation is needed when reusing a subtree from a previous search
        because the value of root nodes and inner nodes have a different
        meaning (position value vs. move values) so the root value cannot be
        reused but all other nodes in the tree can be reused without
        changes.
        It is not thread-safe and may not be called during the search. */
    void init_value(Float value, Float count);

    /** Copy the value and RAVE value from another node without changed
        the chil information.
        This function is not thread-safe and may not be called during the
        search. */
    void copy_data_from(const Node& node);

    void link_children(NodeIndex first_child, int nu_children);

    void unlink_children();

    void add_value(Float v);

    void add_rave_value(Float v, Float weight);

    /** Get node index of first child.
        @pre has_children() */
    NodeIndex get_first_child() const;

private:
    atomic<Float> m_count;

    atomic<Float> m_value;

    atomic<Float> m_rave_count;

    atomic<Float> m_rave_value;

    atomic<unsigned short> m_nu_children;

    Move m_move;

    NodeIndex m_first_child;

    /** Not to be implemented */
    Node(const Node&);

    /** Not to be implemented */
    Node& operator=(const Node&);
};

template<typename M>
inline Node<M>::Node()
{
}

template<typename M>
void Node<M>::add_rave_value(Float v, Float weight)
{
    // Intentionally uses no synchronization and does not care about
    // lost updates in multi-threaded mode
    Float count = m_rave_count.load(memory_order_relaxed);
    Float value = m_rave_value.load(memory_order_relaxed);
    count += weight;
    value += weight * (v - value) / count;
    m_rave_value.store(value, memory_order_relaxed);
    m_rave_count.store(count, memory_order_relaxed);
}

template<typename M>
void Node<M>::add_value(Float v)
{
    // Intentionally uses no synchronization and does not care about
    // lost updates in multi-threaded mode
    Float count = m_count.load(memory_order_relaxed);
    Float value = m_value.load(memory_order_relaxed);
    ++count;
    value +=  (v - value) / count;
    m_value.store(value, memory_order_relaxed);
    m_count.store(count, memory_order_relaxed);
}

template<typename M>
void Node<M>::copy_data_from(const Node& node)
{
    // Reminder to update this function when the class gets additional members
    struct Dummy
    {
        atomic<Float> m_count;
        atomic<Float> m_value;
        atomic<Float> m_rave_count;
        atomic<Float> m_rave_value;
        atomic<unsigned short> m_nu_children;
        Move m_move;
        NodeIndex m_first_child;
    };
    static_assert(sizeof(Node) == sizeof(Dummy), "");

    m_move = node.m_move;
    m_count.store(node.m_count);
    m_value.store(node.m_value);
    m_rave_count.store(node.m_rave_count);
    m_rave_value.store(node.m_rave_value);
}

template<typename M>
inline Float Node<M>::get_count() const
{
    return m_count.load(memory_order_relaxed);
}

template<typename M>
inline NodeIndex Node<M>::get_first_child() const
{
    LIBBOARDGAME_ASSERT(has_children());
    return m_first_child;
}

template<typename M>
inline const typename Node<M>::Move& Node<M>::get_move() const
{
    return m_move;
}

template<typename M>
inline unsigned Node<M>::get_nu_children() const
{
    return m_nu_children.load(memory_order_acquire);
}

template<typename M>
inline Float Node<M>::get_rave_count() const
{
    return m_rave_count.load(memory_order_relaxed);
}

template<typename M>
inline Float Node<M>::get_rave_value() const
{
    return m_rave_value.load(memory_order_relaxed);
}

template<typename M>
inline Float Node<M>::get_value() const
{
    return m_value.load(memory_order_relaxed);
}

template<typename M>
inline bool Node<M>::has_children() const
{
    return get_nu_children() > 0;
}

template<typename M>
void Node<M>::init(const Move& mv, Float value, Float count, Float rave_value,
                   Float rave_count)
{
    // The node is not yet visible to other threads because init() is called
    // before the children are linked to its parent with link_children()
    // (which does a memory_order_release on m_nu_children of the parent).
    // Therefore, the most efficient way here is to initialize all values with
    // memory_order_relaxed.
    m_move = mv;
    m_count.store(count, memory_order_relaxed);
    m_value.store(value, memory_order_relaxed);
    m_rave_count.store(rave_count, memory_order_relaxed);
    m_rave_value.store(rave_value, memory_order_relaxed);
    m_nu_children.store(0, memory_order_relaxed);
}

template<typename M>
void Node<M>::init_value(Float value, Float count)
{
    m_count = count;
    m_value = value;
}

template<typename M>
inline void Node<M>::link_children(NodeIndex first_child, int nu_children)
{
    LIBBOARDGAME_ASSERT(nu_children <= numeric_limits<unsigned short>::max());
    // first_child cannot be 0 because 0 is always used for the root node
    LIBBOARDGAME_ASSERT(first_child != 0);
    m_first_child = first_child;
    m_nu_children.store(static_cast<unsigned short>(nu_children),
                        memory_order_release);
}

template<typename M>
inline void Node<M>::unlink_children()
{
    m_nu_children.store(0, memory_order_release);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_NODE_H
