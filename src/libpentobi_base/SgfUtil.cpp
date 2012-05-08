//-----------------------------------------------------------------------------
/** @file libpentobi_base/SgfUtil.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SgfUtil.h"

#include "libboardgame_util/Assert.h"

namespace libpentobi_base {
namespace sgf_util {

//-----------------------------------------------------------------------------

const char* get_color_id(GameVariant variant, Color c)
{
    if (variant == game_variant_duo || variant == game_variant_junior)
        return (c == Color(0) ? "B" : "W");
    if (c == Color(0))
        return "1";
    if (c == Color(1))
        return "2";
    if (c == Color(2))
        return "3";
    LIBBOARDGAME_ASSERT(c == Color(3));
    return "4";
}

const char* get_setup_id(GameVariant variant, Color c)
{
    if (variant == game_variant_duo || variant == game_variant_junior)
        return (c == Color(0) ? "AB" : "AW");
    if (c == Color(0))
        return "A1";
    if (c == Color(1))
        return "A2";
    if (c == Color(2))
        return "A3";
    LIBBOARDGAME_ASSERT(c == Color(3));
    return "A4";
}

//-----------------------------------------------------------------------------

} // namespace sgf_util
} // namespace libpentobi_base
