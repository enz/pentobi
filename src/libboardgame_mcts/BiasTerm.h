//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/BiasTerm.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_BIAS_TERM_H
#define LIBBOARDGAME_MCTS_BIAS_TERM_H

#include <cmath>
#include "Float.h"
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/FastLog.h"

namespace libboardgame_mcts {

using namespace std;
using libboardgame_util::FastLog;

//-----------------------------------------------------------------------------

/** Computes the UCT bias term.
    Uses a lookup table for small counts. */
class BiasTerm
{
public:
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

inline Float BiasTerm::get(Float child_count) const
{
    LIBBOARDGAME_ASSERT(child_count >= 0);
    Float child_part;
    if (child_count < nu_precomp)
        child_part = m_precomp_child_part[static_cast<unsigned>(child_count)];
    else
        child_part = compute_child_part(child_count);
    return m_parent_part * child_part;
}

inline Float BiasTerm::compute_child_part(Float child_count) const
{
    return sqrt(1 / max(child_count, Float(1)));
}

inline Float BiasTerm::compute_parent_part(Float parent_count) const
{
    if (m_bias_term_constant == 0 || parent_count == 0)
        return 0;
    return m_bias_term_constant * sqrt(m_fast_log.get_log(float(parent_count)));
}

inline Float BiasTerm::get_bias_term_constant() const
{
    return m_bias_term_constant;
}

inline void BiasTerm::start_iteration(Float parent_count)
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
