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
const char* to_string(GameVariant game_variant);

/** Parse name of game variant as in the GM property in Blokus SGF files.
    The parsing is case-insensitive, leading and trailing whitespaced are
    ignored.
    @param s
    @param[out] game_variant
    @result True if the string contained a valid game variant. */
bool parse_game_variant(const string& s, GameVariant& game_variant);

unsigned int get_nu_colors(GameVariant variant);

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_GAME_VARIANT_H
