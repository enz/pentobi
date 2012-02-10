//-----------------------------------------------------------------------------
/** @file mcts/Node.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_NODE_H
#define LIBBOARDGAME_MCTS_NODE_H

#include <limits>
#include "ValueType.h"
#include "libboardgame_util/Assert.h"

namespace libboardgame_mcts {

using namespace std;

template<typename M> class ChildIterator;

//-----------------------------------------------------------------------------

template<typename M>
class Node
{
    friend class ChildIterator<M>;

public:
    typedef M Move;

    Node();

    void init(const Move& mv);

    void init(const Move& mv, ValueType value, ValueType count,
              ValueType rave_value, ValueType rave_count);

    const Move& get_move() const;

    /** Number of values that were added.
        This can be larger than the visit count if prior knowledge is used. */
    ValueType get_count() const;

    /** Value of the node.
        For the root node, this is the value of the position from the point of
        view of the player at the root node; for all other nodes, this is the
        value of the move leading to the position at the node from the point
        of view of the player at the parent node. */
    ValueType get_value() const;

    ValueType get_visit_count() const;

    void inc_visit_count();

    ValueType get_rave_count() const;

    /** RAVE value of the node.
        For the root node, this is the value id undefined; for all other nodes,
        this is the RAVE value of the move leading to the position at the node
        from the point of view of the player at the parent node.
        See @ref libboardgame_doc_rave. */
    ValueType get_rave_value() const;

    bool has_children() const;

    unsigned int get_nu_children() const;

    void clear();

    /** Clears only the value and RAVE value.
        This operation is needed when reusing a subtree from a previous search
        because the value of root nodes and inner nodes have a different meaning
        (position value vs. move values) so the value cannot be reused but the
        child information and visit count should be preserved. */
    void clear_values();

    void link_children(Node<Move>& first_child, int nu_children);

    void unlink_children();

    void add_value(ValueType v);

    void add_rave_value(ValueType v, ValueType weight);

    void copy_data_from(const Node& node);

private:
    unsigned short m_nu_children;

    Move m_move;

    ValueType m_count;

    ValueType m_value;

    ValueType m_rave_count;

    ValueType m_rave_value;

    ValueType m_visit_count;

    Node* m_first_child;

    /** Not to be implemented */
    Node(const Node&);

    /** Not to be implemented */
    Node& operator=(const Node&);

    const Node* get_first_child() const;
};

template<typename M>
inline Node<M>::Node()
{
}

template<typename M>
void Node<M>::add_rave_value(ValueType v, ValueType weight)
{
    ValueType count = m_rave_count;
    count += weight;
    v -= m_rave_value;
    m_rave_value +=  weight * v / count;
    m_rave_count = count;
}

template<typename M>
void Node<M>::add_value(ValueType v)
{
    ValueType count = m_count;
    ++count;
    v -= m_value;
    m_value +=  v / count;
    m_count = count;
}

template<typename M>
void Node<M>::clear()
{
    m_count = 0;
    m_value = 0;
    m_rave_count = 0;
    m_rave_value = 0;
    m_visit_count = 0;
    m_first_child = 0;
}

template<typename M>
void Node<M>::clear_values()
{
    m_count = 0;
    m_value = 0;
    m_rave_count = 0;
    m_rave_value = 0;
}

template<typename M>
void Node<M>::copy_data_from(const Node& node)
{
    // Reminder to update this function when the class gets additional members
    struct Dummy
    {
        unsigned short m_nu_children;
        Move m_move;
        ValueType m_count;
        ValueType m_value;
        ValueType m_rave_count;
        ValueType m_rave_value;
        ValueType m_visit_count;
        Node* m_first_child;
    };
    static_assert(sizeof(Node) == sizeof(Dummy),
                  "libboardgame_mcts::Node::copy_data_from needs updating");

    m_move = node.m_move;
    m_count = node.m_count;
    m_value = node.m_value;
    m_rave_count = node.m_rave_count;
    m_rave_value = node.m_rave_value;
    m_visit_count = node.m_visit_count;
}

template<typename M>
inline ValueType Node<M>::get_count() const
{
    return m_count;
}

template<typename M>
inline const Node<M>* Node<M>::get_first_child() const
{
    return m_first_child;
}

template<typename M>
inline const typename Node<M>::Move& Node<M>::get_move() const
{
    return m_move;
}

template<typename M>
inline unsigned int Node<M>::get_nu_children() const
{
    LIBBOARDGAME_ASSERT(m_first_child != 0);
    return m_nu_children;
}

template<typename M>
inline ValueType Node<M>::get_rave_count() const
{
    return m_rave_count;
}

template<typename M>
inline ValueType Node<M>::get_rave_value() const
{
    LIBBOARDGAME_ASSERT(m_rave_count > 0);
    return m_rave_value;
}

template<typename M>
inline ValueType Node<M>::get_value() const
{
    LIBBOARDGAME_ASSERT(m_count > 0);
    return m_value;
}

template<typename M>
inline ValueType Node<M>::get_visit_count() const
{
    return m_visit_count;
}

template<typename M>
inline bool Node<M>::has_children() const
{
    return m_first_child != 0;
}

template<typename M>
inline void Node<M>::inc_visit_count()
{
    ++m_visit_count;
}

template<typename M>
void Node<M>::init(const Move& mv)
{
    m_move = mv;
    clear();
}

template<typename M>
void Node<M>::init(const Move& mv, ValueType value, ValueType count,
                   ValueType rave_value, ValueType rave_count)
{
    m_move = mv;
    m_count = count;
    m_value = value;
    m_rave_count = rave_count;
    m_rave_value = rave_value;
    m_visit_count = 0;
    m_first_child = 0;
}

template<typename M>
inline void Node<M>::link_children(Node<Move>& first_child, int nu_children)
{
    m_nu_children = nu_children;
    m_first_child = &first_child;
}

template<typename M>
inline void Node<M>::unlink_children()
{
    m_first_child = 0;
}

//-----------------------------------------------------------------------------

template<typename MOVE>
class ChildIterator
{
public:
    ChildIterator(const Node<MOVE>& node);

    operator bool() const;

    void operator++();

    const Node<MOVE>& operator*();

    const Node<MOVE>* operator->();

private:
    const Node<MOVE>* m_current;

    const Node<MOVE>* m_end;
};

template<typename MOVE>
ChildIterator<MOVE>::ChildIterator(const Node<MOVE>& node)
{
    m_current = node.get_first_child();
    if (m_current != 0)
        m_end = m_current + node.get_nu_children();
    else
        m_end = 0;
}

template<typename MOVE>
inline ChildIterator<MOVE>::operator bool() const
{
    return m_current != m_end;
}

template<typename MOVE>
inline void ChildIterator<MOVE>::operator++()
{
    LIBBOARDGAME_ASSERT(operator bool());
    ++m_current;
}

template<typename MOVE>
inline const Node<MOVE>& ChildIterator<MOVE>::operator*()
{
    LIBBOARDGAME_ASSERT(operator bool());
    return *m_current;
}

template<typename MOVE>
inline const Node<MOVE>* ChildIterator<MOVE>::operator->()
{
    LIBBOARDGAME_ASSERT(operator bool());
    return m_current;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_NODE_H
