//-----------------------------------------------------------------------------
/** @file libpentobi_base/GameVariant.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_GAME_VARIANT_H
#define LIBPENTOBI_BASE_GAME_VARIANT_H

#include <string>

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

enum GameVariant
{
    game_variant_classic,

    game_variant_classic_2,

    game_variant_duo,

    game_variant_junior,

    game_variant_trigon,

    game_variant_trigon_2,

    game_variant_trigon_3
};

/** Get name of game variant as in the GM property in Blokus SGF files. */
const char* to_string(GameVariant variant);

/** Get a short lowercase string without spaces that can be used as
    a identifier for a game variant.
    The strings used are "classic", "classic_2", "duo", "trigon", "trigon_2",
    "trigon_3", "junior" */
const char* to_string_id(GameVariant variant);

/** Parse name of game variant as in the GM property in Blokus SGF files.
    The parsing is case-insensitive, leading and trailing whitespaced are
    ignored.
    @param s
    @param[out] game_variant
    @result True if the string contained a valid game variant. */
bool parse_game_variant(const string& s, GameVariant& variant);

/** Parse short lowercase name of game variant as returned to_string_id().
    @param s
    @param[out] game_variant
    @result True if the string contained a valid game variant. */
bool parse_game_variant_id(const string& s, GameVariant& variant);

unsigned int get_nu_colors(GameVariant variant);

unsigned int get_nu_players(GameVariant variant);

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_GAME_VARIANT_H
