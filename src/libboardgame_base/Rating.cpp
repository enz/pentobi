//-----------------------------------------------------------------------------
/** @file libboardgame_base/Rating.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Rating.h"

#include <cmath>
#include "libboardgame_util/Assert.h"

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

float Rating::get_expected_result(Rating elo_opponent,
                                  unsigned int nu_opponents) const
{
    float diff = elo_opponent.m_elo - m_elo;
    return
        1.f
        / (1.f + static_cast<float>(nu_opponents) * pow(10.f, diff / 400.f));
}

void Rating::update_multiplayer(float game_result, Rating elo_opponent,
                    unsigned int nu_opponents, float k_value)
{
    LIBBOARDGAME_ASSERT(k_value > 0);
    float diff = game_result - get_expected_result(elo_opponent, nu_opponents);
    m_elo += k_value * diff;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
