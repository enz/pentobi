//-----------------------------------------------------------------------------
/** @file libpentobi_base/Variant.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Variant.h"

#include "libboardgame_base/RectGeometry.h"
#include "libboardgame_base/TrigonGeometry.h"
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/StringUtil.h"

namespace libpentobi_base {

using libboardgame_base::PointTransfIdent;
using libboardgame_base::PointTransfRefl;
using libboardgame_base::PointTransfReflRot180;
using libboardgame_base::PointTransfRot180;
using libboardgame_base::PointTransfRot270Refl;
using libboardgame_base::PointTransfTrigonReflRot60;
using libboardgame_base::PointTransfTrigonReflRot120;
using libboardgame_base::PointTransfTrigonReflRot240;
using libboardgame_base::PointTransfTrigonReflRot300;
using libboardgame_base::PointTransfTrigonRot60;
using libboardgame_base::PointTransfTrigonRot120;
using libboardgame_base::PointTransfTrigonRot240;
using libboardgame_base::PointTransfTrigonRot300;
using libboardgame_base::RectGeometry;
using libboardgame_base::TrigonGeometry;
using libboardgame_util::trim;
using libboardgame_util::to_lower;

//-----------------------------------------------------------------------------

BoardType get_board_type(Variant variant)
{
    switch (variant)
    {
    case Variant::duo:
    case Variant::junior:
        return BoardType::duo;
    case Variant::classic:
    case Variant::classic_2:
    case Variant::classic_3:
        return BoardType::classic;
    case Variant::trigon:
    case Variant::trigon_2:
        return BoardType::trigon;
    default:
        LIBBOARDGAME_ASSERT(variant == Variant::trigon_3);
        return BoardType::trigon_3;
    }
}

const Geometry& get_geometry(Variant variant)
{
    switch (get_board_type(variant))
    {
    case BoardType::duo:
        return RectGeometry<Point>::get(14, 14);
    case BoardType::classic:
        return RectGeometry<Point>::get(20, 20);
    case BoardType::trigon:
        return TrigonGeometry<Point>::get(9);
    default:
        LIBBOARDGAME_ASSERT(variant == Variant::trigon_3);
        return TrigonGeometry<Point>::get(8);
    }
}

Color::IntType get_nu_colors(Variant variant)
{
    switch (variant)
    {
    case Variant::duo:
    case Variant::junior:
        return 2;
    case Variant::trigon_3:
        return 3;
    default:
        LIBBOARDGAME_ASSERT(variant == Variant::classic
                            || variant == Variant::classic_2
                            || variant == Variant::classic_3
                            || variant == Variant::trigon
                            || variant == Variant::trigon_2);
        return 4;
    }
}

Color::IntType get_nu_players(Variant variant)
{
    switch (variant)
    {
    case Variant::duo:
    case Variant::junior:
    case Variant::classic_2:
    case Variant::trigon_2:
        return 2;
    case Variant::classic_3:
    case Variant::trigon_3:
        return 3;
    default:
        LIBBOARDGAME_ASSERT(variant == Variant::classic
                            || variant == Variant::trigon);
        return 4;
    }
}

void get_transforms(Variant variant,
                    vector<unique_ptr<PointTransform<Point>>>& transforms,
                    vector<unique_ptr<PointTransform<Point>>>& inv_transforms)
{
    transforms.clear();
    inv_transforms.clear();
    switch (get_board_type(variant))
    {
    case BoardType::duo:
        transforms.emplace_back(new PointTransfIdent<Point>);
        inv_transforms.emplace_back(new PointTransfIdent<Point>);
        transforms.emplace_back(new PointTransfRot270Refl<Point>);
        inv_transforms.emplace_back(new PointTransfRot270Refl<Point>);
        break;
    case BoardType::trigon:
        transforms.emplace_back(new PointTransfIdent<Point>);
        inv_transforms.emplace_back(new PointTransfIdent<Point>);
        transforms.emplace_back(new PointTransfTrigonRot60<Point>);
        inv_transforms.emplace_back(new PointTransfTrigonRot300<Point>);
        transforms.emplace_back(new PointTransfTrigonRot120<Point>);
        inv_transforms.emplace_back(new PointTransfTrigonRot240<Point>);
        transforms.emplace_back(new PointTransfRot180<Point>);
        inv_transforms.emplace_back(new PointTransfRot180<Point>);
        transforms.emplace_back(new PointTransfTrigonRot240<Point>);
        inv_transforms.emplace_back(new PointTransfTrigonRot120<Point>);
        transforms.emplace_back(new PointTransfTrigonRot300<Point>);
        inv_transforms.emplace_back(new PointTransfTrigonRot60<Point>);
        transforms.emplace_back(new PointTransfRefl<Point>);
        inv_transforms.emplace_back(new PointTransfRefl<Point>);
        transforms.emplace_back(new PointTransfTrigonReflRot60<Point>);
        inv_transforms.emplace_back(new PointTransfTrigonReflRot60<Point>);
        transforms.emplace_back(new PointTransfTrigonReflRot120<Point>);
        inv_transforms.emplace_back(new PointTransfTrigonReflRot120<Point>);
        transforms.emplace_back(new PointTransfReflRot180<Point>);
        inv_transforms.emplace_back(new PointTransfReflRot180<Point>);
        transforms.emplace_back(new PointTransfTrigonReflRot240<Point>);
        inv_transforms.emplace_back(new PointTransfTrigonReflRot240<Point>);
        transforms.emplace_back(new PointTransfTrigonReflRot300<Point>);
        inv_transforms.emplace_back(new PointTransfTrigonReflRot300<Point>);
        break;
    default:
        transforms.emplace_back(new PointTransfIdent<Point>);
        inv_transforms.emplace_back(new PointTransfIdent<Point>);
    }
}

bool parse_variant(const string& s, Variant& variant)
{
    string t = to_lower(trim(s));
    if (t == "blokus")
        variant = Variant::classic;
    else if (t == "blokus two-player")
        variant = Variant::classic_2;
    else if (t == "blokus three-player")
        variant = Variant::classic_3;
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
    else if (t == "classic_3" || t == "c3")
        variant = Variant::classic_3;
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
    case Variant::classic_3:
        return "Blokus Three-Player";
    case Variant::duo:
        return "Blokus Duo";
    case Variant::junior:
        return "Blokus Junior";
    case Variant::trigon:
        return "Blokus Trigon";
    case Variant::trigon_2:
        return "Blokus Trigon Two-Player";
    default:
        LIBBOARDGAME_ASSERT(variant == Variant::trigon_3);
        return "Blokus Trigon Three-Player";
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
    case Variant::classic_3:
        return "classic_3";
    case Variant::duo:
        return "duo";
    case Variant::junior:
        return "junior";
    case Variant::trigon:
        return "trigon";
    case Variant::trigon_2:
        return "trigon_2";
    default:
        LIBBOARDGAME_ASSERT(variant == Variant::trigon_3);
        return "trigon_3";
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
