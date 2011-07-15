//-----------------------------------------------------------------------------
/** @file UctBiasTerm.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_UCT_BIAS_TERM_H
#define LIBBOARDGAME_MCTS_UCT_BIAS_TERM_H

#include <cmath>
#include "Node.h"
#include "libboardgame_util/FastLog.h"

namespace libboardgame_mcts {

using libboardgame_util::FastLog;

//-----------------------------------------------------------------------------

/** Bias term used in UCT.
    The UCT bias term has the form @f$ c \sqrt{\frac{\log{n}}{T_j(n) + 1}} @f$
    with the given constant c. Note that this slightly differs from the one
    described in @ref libboardgame_doc_uct by adding 1 to the action count, such
    that the bias term can be used for actions that have not been explored at
    all. For larger counts, the difference will be negligable. */
template<class M>
class UctBiasTerm
{
public:
    typedef M Move;

    typedef libboardgame_mcts::Node<M> Node;

    UctBiasTerm();

    void begin_select_child(const Node& parent);

    ValueType get(const Node& child);

    /** Constant used in UCT bias term. */
    void set_bias_term_constant(ValueType c);

    ValueType get_bias_term_constant() const;

private:
    ValueType m_bias_term_constant;

    ValueType m_bias_term_constant_sq;

    ValueType m_constant_part;

    FastLog m_fast_log;
};

template<class M>
UctBiasTerm<M>::UctBiasTerm()
    : m_fast_log(10)
{
    set_bias_term_constant(ValueType(0.7));
}

template<class M>
inline void UctBiasTerm<M>::begin_select_child(const Node& parent)
{
    if (m_bias_term_constant != 0)
    {
        ValueType count = parent.get_count();
        LIBBOARDGAME_ASSERT(count > 0);
        m_constant_part =
            m_bias_term_constant_sq * m_fast_log.get_log(float(count));
    }
}

template<class M>
inline ValueType UctBiasTerm<M>::get(const Node& child)
{
    if (m_bias_term_constant != 0)
    {
        ValueType count = child.get_count();
        return sqrt(m_constant_part / (count + 1));
    }
    else
        return 0;
}

template<class M>
inline ValueType UctBiasTerm<M>::get_bias_term_constant() const
{
    return m_bias_term_constant;
}

template<class M>
inline void UctBiasTerm<M>::set_bias_term_constant(ValueType c)
{
    m_bias_term_constant = c;
    m_bias_term_constant_sq = c * c;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_UCT_BIAS_TERM_H
