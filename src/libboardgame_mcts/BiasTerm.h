//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/BiasTerm.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_BIAS_TERM_H
#define LIBBOARDGAME_MCTS_BIAS_TERM_H

#include <cmath>
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/FastLog.h"

namespace libboardgame_mcts {

using namespace std;
using libboardgame_util::FastLog;

//-----------------------------------------------------------------------------

/** Computes the UCT bias term.
    Fractional counts are rounded to integer count (only for small counts).
    For child counts smaller than 1, the same value is returned as as for child
    count 1 but nodes should be initialized with prior knowledge anyway so that
    child count 0 will not occur. */
template<typename F>
class BiasTerm
{
public:
    typedef F Float;

    BiasTerm(Float bias_term_constant);

    void set_bias_term_constant(Float value);

    Float get_bias_term_constant() const;

    void start_iteration(Float parent_count);

    Float get(Float child_count) const;

private:
    static const unsigned nu_precomp = 50;

    /** The part of the bias term that does not depend on the child count. */
    Float m_parent_part;

    Float m_bias_term_constant;

    Float m_precomp_parent_part[nu_precomp];

    Float m_precomp_child_part[nu_precomp];

    FastLog m_fast_log;

    Float compute_parent_part(Float parent_count) const;

    Float compute_child_part(Float child_count) const;
};

template<typename F>
BiasTerm<F>::BiasTerm(Float bias_term_constant)
    : m_fast_log(10)
{
    set_bias_term_constant(bias_term_constant);
}

template<typename F>
inline auto BiasTerm<F>::get(Float child_count) const -> Float
{
    LIBBOARDGAME_ASSERT(child_count >= 0);
    if (child_count <= 1)
        return m_parent_part;
    Float child_part;
    if (child_count < nu_precomp)
        child_part = m_precomp_child_part[static_cast<unsigned>(child_count)];
    else
        child_part = compute_child_part(child_count);
    return m_parent_part * child_part;
}

template<typename F>
inline auto BiasTerm<F>::compute_child_part(Float child_count) const -> Float
{
    LIBBOARDGAME_ASSERT(child_count > 0);
    return sqrt(1 / child_count);
}

template<typename F>
inline auto BiasTerm<F>::compute_parent_part(Float parent_count) const -> Float
{
    if (m_bias_term_constant == 0 || parent_count == 0)
        return 0;
    return
        m_bias_term_constant * sqrt(m_fast_log.get_log(float(parent_count)));
}

template<typename F>
inline auto BiasTerm<F>::get_bias_term_constant() const -> Float
{
    return m_bias_term_constant;
}

template<typename F>
void BiasTerm<F>::set_bias_term_constant(Float value)
{
    m_bias_term_constant = value;
    for (unsigned i = 0; i < nu_precomp; ++i)
        m_precomp_parent_part[i] = compute_parent_part(static_cast<Float>(i));
    for (unsigned i = 1; i < nu_precomp; ++i)
        m_precomp_child_part[i] = compute_child_part(static_cast<Float>(i));
}

template<typename F>
inline void BiasTerm<F>::start_iteration(Float parent_count)
{
    LIBBOARDGAME_ASSERT(parent_count >= 0);
    if (parent_count < nu_precomp)
        m_parent_part =
            m_precomp_parent_part[static_cast<unsigned>(parent_count)];
    else
        m_parent_part = compute_parent_part(parent_count);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_BIAS_TERM_H
