//-----------------------------------------------------------------------------
/** @file libpentobi_base/Variant.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Variant.h"

#include "libboardgame_util/Assert.h"
#include "libboardgame_util/StringUtil.h"

namespace libpentobi_base {

using libboardgame_util::trim;
using libboardgame_util::to_lower;

//-----------------------------------------------------------------------------

Color::IntType get_nu_colors(Variant variant)
{
    switch (variant)
    {
    case Variant::duo:
    case Variant::junior:
        return 2;
    case Variant::trigon_3:
        return 3;
    case Variant::classic:
    case Variant::classic_2:
    case Variant::trigon:
    case Variant::trigon_2:
        return 4;
    }
    LIBBOARDGAME_ASSERT(false);
    return 0;
}

unsigned get_nu_players(Variant variant)
{
    switch (variant)
    {
    case Variant::duo:
    case Variant::junior:
    case Variant::classic_2:
    case Variant::trigon_2:
        return 2;
    case Variant::trigon_3:
        return 3;
    case Variant::classic:
    case Variant::trigon:
        return 4;
    }
    LIBBOARDGAME_ASSERT(false);
    return 0;
}

bool parse_variant(const string& s, Variant& variant)
{
    string t = to_lower(trim(s));
    if (t == "blokus")
        variant = Variant::classic;
    else if (t == "blokus two-player")
        variant = Variant::classic_2;
    else if (t == "blokus trigon")
        variant = Variant::trigon;
    else if (t == "blokus trigon two-player")
        variant = Variant::trigon_2;
    else if (t == "blokus trigon three-player")
        variant = Variant::trigon_3;
    else if (t == "blokus duo")
        variant = Variant::duo;
    else if (t == "blokus junior")
        variant = Variant::junior;
    else
        return false;
    return true;
}

bool parse_variant_id(const string& s, Variant& variant)
{
    string t = to_lower(trim(s));
    if (t == "classic" || t == "c")
        variant = Variant::classic;
    else if (t == "classic_2" || t == "c2")
        variant = Variant::classic_2;
    else if (t == "trigon" || t == "t")
        variant = Variant::trigon;
    else if (t == "trigon_2" || t == "t2")
        variant = Variant::trigon_2;
    else if (t == "trigon_3" || t == "t3")
        variant = Variant::trigon_3;
    else if (t == "duo" || t == "d")
        variant = Variant::duo;
    else if (t == "junior" || t == "j")
        variant = Variant::junior;
    else
        return false;
    return true;
}

const char* to_string(Variant variant)
{
    switch (variant)
    {
    case Variant::classic:
        return "Blokus";
    case Variant::classic_2:
        return "Blokus Two-Player";
    case Variant::duo:
        return "Blokus Duo";
    case Variant::junior:
        return "Blokus Junior";
    case Variant::trigon:
        return "Blokus Trigon";
    case Variant::trigon_2:
        return "Blokus Trigon Two-Player";
    case Variant::trigon_3:
        return "Blokus Trigon Three-Player";
    default:
        LIBBOARDGAME_ASSERT(false);
        return "?";
    }
}

const char* to_string_id(Variant variant)
{
    switch (variant)
    {
    case Variant::classic:
        return "classic";
    case Variant::classic_2:
        return "classic_2";
    case Variant::duo:
        return "duo";
    case Variant::junior:
        return "junior";
    case Variant::trigon:
        return "trigon";
    case Variant::trigon_2:
        return "trigon_2";
    case Variant::trigon_3:
        return "trigon_3";
    default:
        LIBBOARDGAME_ASSERT(false);
        return "?";
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
