//-----------------------------------------------------------------------------
/** @file libboardgame_base/Rating.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Rating.h"

#include <cmath>
#include <iostream>
#include "libboardgame_util/Assert.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

ostream& operator<<(ostream& out, const Rating& rating)
{
    out << rating.m_elo;
    return out;
}

istream& operator>>(istream& in, Rating& rating)
{
    in >> rating.m_elo;
    return in;
}

float Rating::get_expected_result(Rating elo_opponent,
                                  unsigned nu_opponents) const
{
    float diff = elo_opponent.m_elo - m_elo;
    return
        1.f
        / (1.f + static_cast<float>(nu_opponents) * pow(10.f, diff / 400.f));
}

void Rating::update(float game_result, Rating elo_opponent, float k_value,
                    unsigned nu_opponents)
{
    LIBBOARDGAME_ASSERT(k_value > 0);
    float diff = game_result - get_expected_result(elo_opponent, nu_opponents);
    m_elo += k_value * diff;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
