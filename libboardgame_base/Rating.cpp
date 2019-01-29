//-----------------------------------------------------------------------------
/** @file libboardgame_base/Rating.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Rating.h"

#include <iostream>
#include "libboardgame_util/Assert.h"

namespace libboardgame_base {

//-----------------------------------------------------------------------------

ostream& operator<<(ostream& out, Rating rating)
{
    out << rating.m_elo;
    return out;
}

istream& operator>>(istream& in, Rating& rating)
{
    in >> rating.m_elo;
    return in;
}

double Rating::get_expected_result(Rating elo_opponent,
                                  unsigned nu_opponents) const
{
    auto diff = elo_opponent.m_elo - m_elo;
    return
        1. / (1. + static_cast<double>(nu_opponents) * pow(10., diff / 400.));
}

void Rating::update(double game_result, Rating elo_opponent, double k_value,
                    unsigned nu_opponents)
{
    LIBBOARDGAME_ASSERT(k_value > 0);
    auto diff = game_result - get_expected_result(elo_opponent, nu_opponents);
    m_elo += k_value * diff;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
