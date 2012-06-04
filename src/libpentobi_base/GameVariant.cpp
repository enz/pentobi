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

unsigned int get_nu_colors(GameVariant variant)
{
    switch (variant)
    {
    case game_variant_duo:
    case game_variant_junior:
        return 2;
    case game_variant_trigon_3:
        return 3;
    case game_variant_classic:
    case game_variant_classic_2:
    case game_variant_trigon:
    case game_variant_trigon_2:
        return 4;
    }
    LIBBOARDGAME_ASSERT(false);
    return 0;
}

unsigned int get_nu_players(GameVariant variant)
{
    switch (variant)
    {
    case game_variant_duo:
    case game_variant_junior:
    case game_variant_classic_2:
    case game_variant_trigon_2:
        return 2;
    case game_variant_trigon_3:
        return 3;
    case game_variant_classic:
    case game_variant_trigon:
        return 4;
    }
    LIBBOARDGAME_ASSERT(false);
    return 0;
}

bool parse_game_variant(const string& s, GameVariant& variant)
{
    string t = to_lower_copy(trim_copy(s));
    if (t == "blokus")
        variant = game_variant_classic;
    else if (t == "blokus two-player")
        variant = game_variant_classic_2;
    else if (t == "blokus trigon")
        variant = game_variant_trigon;
    else if (t == "blokus trigon two-player")
        variant = game_variant_trigon_2;
    else if (t == "blokus trigon three-player")
        variant = game_variant_trigon_3;
    else if (t == "blokus duo")
        variant = game_variant_duo;
    else if (t == "blokus junior")
        variant = game_variant_junior;
    else
        return false;
    return true;
}

bool parse_game_variant_id(const string& s, GameVariant& variant)
{
    string t = to_lower_copy(trim_copy(s));
    if (t == "classic")
        variant = game_variant_classic;
    else if (t == "classic_2")
        variant = game_variant_classic_2;
    else if (t == "trigon")
        variant = game_variant_trigon;
    else if (t == "trigon_2")
        variant = game_variant_trigon_2;
    else if (t == "trigon_3")
        variant = game_variant_trigon_3;
    else if (t == "duo")
        variant = game_variant_duo;
    else if (t == "junior")
        variant = game_variant_junior;
    else
        return false;
    return true;
}

const char* to_string(GameVariant variant)
{
    switch (variant)
    {
    case game_variant_classic:
        return "Blokus";
    case game_variant_classic_2:
        return "Blokus Two-Player";
    case game_variant_duo:
        return "Blokus Duo";
    case game_variant_junior:
        return "Blokus Junior";
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

const char* to_string_id(GameVariant variant)
{
    switch (variant)
    {
    case game_variant_classic:
        return "classic";
    case game_variant_classic_2:
        return "classic_2";
    case game_variant_duo:
        return "duo";
    case game_variant_junior:
        return "junior";
    case game_variant_trigon:
        return "trigon";
    case game_variant_trigon_2:
        return "trigon_2";
    case game_variant_trigon_3:
        return "trigon_3";
    default:
        LIBBOARDGAME_ASSERT(false);
        return "?";
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
