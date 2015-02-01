//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/BiasTerm.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_BIAS_TERM_H
#define LIBBOARDGAME_MCTS_BIAS_TERM_H

#include <cmath>
#include "libboardgame_util/Assert.h"

namespace libboardgame_mcts {

using namespace std;

//-----------------------------------------------------------------------------

/** Computes the UCT bias term.
    For better speed, log_2 of the parent count is used and rounded to integers
    because then computing the logarithm of a floating point is simply taking
    the exponent. This does not affect convergence guarantees of UCT but
    changes the meaning of the bias term constant by a constant factor.
    Fractional child counts are rounded to integer count (for small counts).
    For child counts smaller than 1, the same value is returned as as for child
    count 1 but nodes should be initialized with prior knowledge anyway so that
    child count 0 will not occur in practice. */
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
    static const unsigned nu_precomp_parent = 30;

    static const unsigned nu_precomp_child = 50;

    static int get_rounded_log_2(Float x);

    /** The part of the bias term that does not depend on the child count. */
    Float m_parent_part;

    Float m_bias_term_constant;

    Float m_precomp_parent_part[nu_precomp_parent];

    Float m_precomp_child_part[nu_precomp_child];

    Float compute_parent_part(int log_parent_count) const;

    Float compute_child_part(Float child_count) const;
};

template<typename F>
BiasTerm<F>::BiasTerm(Float bias_term_constant)
{
    set_bias_term_constant(bias_term_constant);
}

template<typename F>
inline auto BiasTerm<F>::get(Float child_count) const -> Float
{
    LIBBOARDGAME_ASSERT(child_count >= 0);
    Float child_part;
    if (child_count < nu_precomp_child)
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
inline auto BiasTerm<F>::compute_parent_part(
        int log_parent_count) const -> Float
{
    return m_bias_term_constant * sqrt(static_cast<Float>(log_parent_count));
}

template<typename F>
inline auto BiasTerm<F>::get_bias_term_constant() const -> Float
{
    return m_bias_term_constant;
}

template<typename F>
inline int BiasTerm<F>::get_rounded_log_2(Float x)
{
    // Optimized implementation for IEEE-754
    if (numeric_limits<Float>::is_iec559 && sizeof(Float) == 4)
    {
        union
        {
            int32_t i;
            float f;
        } convert;
        convert.f = x;
        // The last added 1 is to be compatible with frexp()
        return ((convert.i >> 23) & 0xff) - 127 + 1;
    }
    else
    {
        // Portable fallback. Slower because frexp is not inline and computes
        // the significant, which we don't need.
        int exponent;
        frexp(x, &exponent);
        return exponent;
    }
}

template<typename F>
void BiasTerm<F>::set_bias_term_constant(Float value)
{
    m_bias_term_constant = value;
    for (unsigned i = 1; i < nu_precomp_parent; ++i)
        m_precomp_parent_part[i] = compute_parent_part(i);
    for (unsigned i = 0; i < nu_precomp_child; ++i)
        m_precomp_child_part[i] =
            compute_child_part(static_cast<Float>(max(i, 1u)));
}

template<typename F>
inline void BiasTerm<F>::start_iteration(Float parent_count)
{
    LIBBOARDGAME_ASSERT(parent_count >= 0);
    if (parent_count < 1)
        m_parent_part = 0;
    else
    {
        int log_2 = get_rounded_log_2(parent_count);
        if (static_cast<unsigned>(log_2) < nu_precomp_parent)
            m_parent_part = m_precomp_parent_part[log_2];
        else
            m_parent_part = compute_parent_part(log_2);
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_BIAS_TERM_H
