//-----------------------------------------------------------------------------
/** @file libpentobi_base/GameVariant.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "GameVariant.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "libboardgame_util/Assert.h"

namespace libpentobi_base {

using boost::trim_copy;
using boost::algorithm::to_lower_copy;

//-----------------------------------------------------------------------------

bool parse_game_variant(const string& s, GameVariant& game_variant)
{
    string t = to_lower_copy(trim_copy(s));
    if (t == "blokus")
        game_variant = game_variant_classic;
    else if (t == "blokus two-player")
        game_variant = game_variant_classic_2;
    else if (t == "blokus trigon")
        game_variant = game_variant_trigon;
    else if (t == "blokus trigon two-player")
        game_variant = game_variant_trigon_2;
    else if (t == "blokus trigon three-player")
        game_variant = game_variant_trigon_3;
    else if (t == "blokus duo")
        game_variant = game_variant_duo;
    else
        return false;
    return true;
}

const char* to_string(GameVariant game_variant)
{
    switch (game_variant)
    {
    case game_variant_classic:
        return "Blokus";
    case game_variant_classic_2:
        return "Blokus Two-Player";
    case game_variant_duo:
        return "Blokus Duo";
    case game_variant_trigon:
        return "Blokus Trigon";
    case game_variant_trigon_2:
        return "Blokus Trigon Two-Player";
    case game_variant_trigon_3:
        return "Blokus Trigon Three-Player";
    default:
        LIBBOARDGAME_ASSERT(false);
        return "?";
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
