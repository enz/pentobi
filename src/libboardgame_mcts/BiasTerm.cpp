//-----------------------------------------------------------------------------
/** @file libboardgame_base/BiasTerm.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BiasTerm.h"

namespace libboardgame_mcts {

using namespace std;

//-----------------------------------------------------------------------------

BiasTerm::BiasTerm(Float bias_term_constant)
    : m_fast_log(10)
{
    set_bias_term_constant(bias_term_constant);
}

void BiasTerm::set_bias_term_constant(Float value)
{
    m_bias_term_constant = value;
    for (unsigned i = 0; i < nu_precomp; ++i)
    {
        m_precomp_parent_part[i] = compute_parent_part(i);
        m_precomp_child_part[i] = compute_child_part(i);
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts
