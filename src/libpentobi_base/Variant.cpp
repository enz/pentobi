//-----------------------------------------------------------------------------
/** @file libpentobi_base/Variant.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Variant.h"

#include "NexosGeometry.h"
#include "TrigonGeometry.h"
#include "libboardgame_base/RectGeometry.h"
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
using libboardgame_util::trim;
using libboardgame_util::to_lower;

//-----------------------------------------------------------------------------

BoardType get_board_type(Variant variant)
{
    BoardType result = BoardType::classic; // Init to avoid compiler warning
    switch (variant)
    {
    case Variant::duo:
    case Variant::junior:
        result = BoardType::duo;
        break;
    case Variant::classic:
    case Variant::classic_2:
    case Variant::classic_3:
        result = BoardType::classic;
        break;
    case Variant::trigon:
    case Variant::trigon_2:
        result = BoardType::trigon;
        break;
    case Variant::trigon_3:
        result = BoardType::trigon_3;
        break;
    case Variant::nexos:
    case Variant::nexos_2:
        result = BoardType::nexos;
        break;
    }
    return result;
}

const Geometry& get_geometry(BoardType board_type)
{
    const Geometry* result = nullptr; // Init to avoid compiler warning
    switch (board_type)
    {
    case BoardType::duo:
        result = &RectGeometry<Point>::get(14, 14);
        break;
    case BoardType::classic:
        result = &RectGeometry<Point>::get(20, 20);
        break;
    case BoardType::trigon:
        result = &TrigonGeometry::get(9);
        break;
    case BoardType::trigon_3:
        result = &TrigonGeometry::get(8);
        break;
    case BoardType::nexos:
        result = &NexosGeometry::get(13);
        break;
    }
    return *result;
}

const Geometry& get_geometry(Variant variant)
{
    return get_geometry(get_board_type(variant));
}

Color::IntType get_nu_colors(Variant variant)
{
    Color::IntType result = 0; // Init to avoid compiler warning
    switch (variant)
    {
    case Variant::duo:
    case Variant::junior:
        result = 2;
        break;
    case Variant::trigon_3:
        result = 3;
        break;
    case Variant::classic:
    case Variant::classic_2:
    case Variant::classic_3:
    case Variant::trigon:
    case Variant::trigon_2:
    case Variant::nexos:
    case Variant::nexos_2:
        result = 4;
        break;
    }
    return result;
}

Color::IntType get_nu_players(Variant variant)
{
    Color::IntType result = 0; // Init to avoid compiler warning
    switch (variant)
    {
    case Variant::duo:
    case Variant::junior:
    case Variant::classic_2:
    case Variant::trigon_2:
    case Variant::nexos_2:
        result = 2;
        break;
    case Variant::classic_3:
    case Variant::trigon_3:
        result = 3;
        break;
    case Variant::classic:
    case Variant::trigon:
    case Variant::nexos:
        result = 4;
        break;
    }
    return result;
}

PieceSet get_piece_set(Variant variant)
{
    PieceSet result = PieceSet::classic; // Init to avoid compiler warning
    switch (variant)
    {
    case Variant::classic:
    case Variant::classic_2:
    case Variant::classic_3:
    case Variant::duo:
        result = PieceSet::classic;
        break;
    case Variant::trigon:
    case Variant::trigon_2:
    case Variant::trigon_3:
        result = PieceSet::trigon;
        break;
    case Variant::junior:
        result = PieceSet::junior;
        break;
    case Variant::nexos:
    case Variant::nexos_2:
        result = PieceSet::nexos;
        break;
    }
    return result;
}

void get_transforms(Variant variant,
                    vector<unique_ptr<PointTransform<Point>>>& transforms,
                    vector<unique_ptr<PointTransform<Point>>>& inv_transforms)
{
    transforms.clear();
    inv_transforms.clear();
    transforms.emplace_back(new PointTransfIdent<Point>);
    inv_transforms.emplace_back(new PointTransfIdent<Point>);
    switch (get_board_type(variant))
    {
    case BoardType::duo:
        transforms.emplace_back(new PointTransfRot270Refl<Point>);
        inv_transforms.emplace_back(new PointTransfRot270Refl<Point>);
        break;
    case BoardType::trigon:
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
    case BoardType::classic:
    case BoardType::trigon_3:
    case BoardType::nexos:
        break;
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
    else if (t == "nexos")
        variant = Variant::nexos;
    else if (t == "nexos two-player")
        variant = Variant::nexos_2;
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
    else if (t == "nexos" || t == "n")
        variant = Variant::nexos;
    else if (t == "nexos_2" || t == "n2")
        variant = Variant::nexos_2;
    else
        return false;
    return true;
}

const char* to_string(Variant variant)
{
    const char* result = nullptr; // Init to avoid compiler warning
    switch (variant)
    {
    case Variant::classic:
        result = "Blokus";
        break;
    case Variant::classic_2:
        result = "Blokus Two-Player";
        break;
    case Variant::classic_3:
        result = "Blokus Three-Player";
        break;
    case Variant::duo:
        result = "Blokus Duo";
        break;
    case Variant::junior:
        result = "Blokus Junior";
        break;
    case Variant::trigon:
        result = "Blokus Trigon";
        break;
    case Variant::trigon_2:
        result = "Blokus Trigon Two-Player";
        break;
    case Variant::trigon_3:
        result = "Blokus Trigon Three-Player";
        break;
    case Variant::nexos:
        result = "Nexos";
        break;
    case Variant::nexos_2:
        result = "Nexos Two-Player";
    }
    return result;
}

const char* to_string_id(Variant variant)
{
    const char* result = nullptr; // Init to avoid compiler warning
    switch (variant)
    {
    case Variant::classic:
        result = "classic";
        break;
    case Variant::classic_2:
        result = "classic_2";
        break;
    case Variant::classic_3:
        result = "classic_3";
        break;
    case Variant::duo:
        result = "duo";
        break;
    case Variant::junior:
        result = "junior";
        break;
    case Variant::trigon:
        result = "trigon";
        break;
    case Variant::trigon_2:
        result = "trigon_2";
        break;
    case Variant::trigon_3:
        result = "trigon_3";
        break;
    case Variant::nexos:
        result = "nexos";
        break;
    case Variant::nexos_2:
        result = "nexos_2";
        break;
    }
    return result;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
