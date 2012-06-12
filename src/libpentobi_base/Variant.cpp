//-----------------------------------------------------------------------------
/** @file libpentobi_base/Variant.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Variant.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "libboardgame_util/Assert.h"

namespace libpentobi_base {

using boost::trim_copy;
using boost::algorithm::to_lower_copy;

//-----------------------------------------------------------------------------

unsigned int get_nu_colors(Variant variant)
{
    switch (variant)
    {
    case variant_duo:
    case variant_junior:
        return 2;
    case variant_trigon_3:
        return 3;
    case variant_classic:
    case variant_classic_2:
    case variant_trigon:
    case variant_trigon_2:
        return 4;
    }
    LIBBOARDGAME_ASSERT(false);
    return 0;
}

unsigned int get_nu_players(Variant variant)
{
    switch (variant)
    {
    case variant_duo:
    case variant_junior:
    case variant_classic_2:
    case variant_trigon_2:
        return 2;
    case variant_trigon_3:
        return 3;
    case variant_classic:
    case variant_trigon:
        return 4;
    }
    LIBBOARDGAME_ASSERT(false);
    return 0;
}

bool parse_variant(const string& s, Variant& variant)
{
    string t = to_lower_copy(trim_copy(s));
    if (t == "blokus")
        variant = variant_classic;
    else if (t == "blokus two-player")
        variant = variant_classic_2;
    else if (t == "blokus trigon")
        variant = variant_trigon;
    else if (t == "blokus trigon two-player")
        variant = variant_trigon_2;
    else if (t == "blokus trigon three-player")
        variant = variant_trigon_3;
    else if (t == "blokus duo")
        variant = variant_duo;
    else if (t == "blokus junior")
        variant = variant_junior;
    else
        return false;
    return true;
}

bool parse_variant_id(const string& s, Variant& variant)
{
    string t = to_lower_copy(trim_copy(s));
    if (t == "classic")
        variant = variant_classic;
    else if (t == "classic_2")
        variant = variant_classic_2;
    else if (t == "trigon")
        variant = variant_trigon;
    else if (t == "trigon_2")
        variant = variant_trigon_2;
    else if (t == "trigon_3")
        variant = variant_trigon_3;
    else if (t == "duo")
        variant = variant_duo;
    else if (t == "junior")
        variant = variant_junior;
    else
        return false;
    return true;
}

const char* to_string(Variant variant)
{
    switch (variant)
    {
    case variant_classic:
        return "Blokus";
    case variant_classic_2:
        return "Blokus Two-Player";
    case variant_duo:
        return "Blokus Duo";
    case variant_junior:
        return "Blokus Junior";
    case variant_trigon:
        return "Blokus Trigon";
    case variant_trigon_2:
        return "Blokus Trigon Two-Player";
    case variant_trigon_3:
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
    case variant_classic:
        return "classic";
    case variant_classic_2:
        return "classic_2";
    case variant_duo:
        return "duo";
    case variant_junior:
        return "junior";
    case variant_trigon:
        return "trigon";
    case variant_trigon_2:
        return "trigon_2";
    case variant_trigon_3:
        return "trigon_3";
    default:
        LIBBOARDGAME_ASSERT(false);
        return "?";
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
