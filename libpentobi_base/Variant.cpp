//-----------------------------------------------------------------------------
/** @file libpentobi_base/Variant.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Variant.h"

#include "CallistoGeometry.h"
#include "GembloQGeometry.h"
#include "NexosGeometry.h"
#include "TrigonGeometry.h"
#include "libboardgame_base/RectGeometry.h"
#include "libboardgame_base/StringUtil.h"

namespace libpentobi_base {

using libboardgame_base::trim;
using libboardgame_base::to_lower;
using libboardgame_base::PointTransfIdent;
using libboardgame_base::PointTransfRefl;
using libboardgame_base::PointTransfReflRot180;
using libboardgame_base::PointTransfRot90;
using libboardgame_base::PointTransfRot180;
using libboardgame_base::PointTransfRot270;
using libboardgame_base::PointTransfRot90Refl;
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
    case Variant::callisto:
    case Variant::callisto_2_4:
        result = BoardType::callisto;
        break;
    case Variant::callisto_2:
        result = BoardType::callisto_2;
        break;
    case Variant::callisto_3:
        result = BoardType::callisto_3;
        break;
    case Variant::gembloq:
    case Variant::gembloq_2_4:
        result = BoardType::gembloq;
        break;
    case Variant::gembloq_2:
        result = BoardType::gembloq_2;
        break;
    case Variant::gembloq_3:
        result = BoardType::gembloq_3;
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
        result = &NexosGeometry::get();
        break;
    case BoardType::callisto:
        result = &CallistoGeometry::get(4);
        break;
    case BoardType::callisto_2:
        result = &CallistoGeometry::get(2);
        break;
    case BoardType::callisto_3:
        result = &CallistoGeometry::get(3);
        break;
    case BoardType::gembloq:
        result = &GembloQGeometry::get(4);
        break;
    case BoardType::gembloq_2:
        result = &GembloQGeometry::get(2);
        break;
    case BoardType::gembloq_3:
        result = &GembloQGeometry::get(3);
        break;
    }
    return *result;
}

const Geometry& get_geometry(Variant variant)
{
    return get_geometry(get_board_type(variant));
}

GeometryType get_geometry_type(Variant variant)
{
    GeometryType result = GeometryType::classic; // Init to avoid compiler warning
    switch (variant)
    {
    case Variant::classic:
    case Variant::classic_2:
    case Variant::classic_3:
    case Variant::duo:
    case Variant::junior:
        result = GeometryType::classic;
        break;
    case Variant::trigon:
    case Variant::trigon_2:
    case Variant::trigon_3:
        result = GeometryType::trigon;
        break;
    case Variant::nexos:
    case Variant::nexos_2:
        result = GeometryType::nexos;
        break;
    case Variant::callisto:
    case Variant::callisto_2:
    case Variant::callisto_2_4:
    case Variant::callisto_3:
        result = GeometryType::callisto;
        break;
    case Variant::gembloq:
    case Variant::gembloq_2:
    case Variant::gembloq_2_4:
    case Variant::gembloq_3:
        result = GeometryType::gembloq;
        break;
    }
    return result;
}

Color::IntType get_nu_colors(Variant variant)
{
    Color::IntType result = 0; // Init to avoid compiler warning
    switch (variant)
    {
    case Variant::duo:
    case Variant::junior:
    case Variant::callisto_2:
    case Variant::gembloq_2:
        result = 2;
        break;
    case Variant::trigon_3:
    case Variant::callisto_3:
    case Variant::gembloq_3:
        result = 3;
        break;
    case Variant::classic:
    case Variant::classic_2:
    case Variant::classic_3:
    case Variant::trigon:
    case Variant::trigon_2:
    case Variant::nexos:
    case Variant::nexos_2:
    case Variant::callisto:
    case Variant::callisto_2_4:
    case Variant::gembloq:
    case Variant::gembloq_2_4:
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
    case Variant::callisto_2:
    case Variant::callisto_2_4:
    case Variant::gembloq_2:
    case Variant::gembloq_2_4:
        result = 2;
        break;
    case Variant::classic_3:
    case Variant::trigon_3:
    case Variant::callisto_3:
    case Variant::gembloq_3:
        result = 3;
        break;
    case Variant::classic:
    case Variant::trigon:
    case Variant::nexos:
    case Variant::callisto:
    case Variant::gembloq:
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
    case Variant::callisto:
    case Variant::callisto_2:
    case Variant::callisto_2_4:
    case Variant::callisto_3:
        result = PieceSet::callisto;
        break;
    case Variant::gembloq:
    case Variant::gembloq_2:
    case Variant::gembloq_2_4:
    case Variant::gembloq_3:
        result = PieceSet::gembloq;
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
    case BoardType::callisto_2:
    case BoardType::callisto:
    case BoardType::callisto_3:
        transforms.emplace_back(new PointTransfRot90<Point>);
        inv_transforms.emplace_back(new PointTransfRot270<Point>);
        transforms.emplace_back(new PointTransfRot180<Point>);
        inv_transforms.emplace_back(new PointTransfRot180<Point>);
        transforms.emplace_back(new PointTransfRot270<Point>);
        inv_transforms.emplace_back(new PointTransfRot90<Point>);
        transforms.emplace_back(new PointTransfRefl<Point>);
        inv_transforms.emplace_back(new PointTransfRefl<Point>);
        transforms.emplace_back(new PointTransfReflRot180<Point>);
        inv_transforms.emplace_back(new PointTransfReflRot180<Point>);
        transforms.emplace_back(new PointTransfRot90Refl<Point>);
        inv_transforms.emplace_back(new PointTransfRot90Refl<Point>);
        transforms.emplace_back(new PointTransfRot270Refl<Point>);
        inv_transforms.emplace_back(new PointTransfRot270Refl<Point>);
        break;
    case BoardType::classic:
    case BoardType::gembloq:
    case BoardType::gembloq_2:
    case BoardType::gembloq_3:
    case BoardType::nexos:
    case BoardType::trigon_3:
        break;
    }
}

bool has_central_symmetry(Variant variant)
{
    return variant == Variant::duo || variant == Variant::junior
            || variant == Variant::trigon_2 || variant == Variant::callisto_2
            || variant == Variant::gembloq_2;
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
    else if (t == "callisto")
        variant = Variant::callisto;
    else if (t == "callisto two-player")
        variant = Variant::callisto_2;
    else if (t == "callisto two-player four-color")
        variant = Variant::callisto_2_4;
    else if (t == "callisto three-player")
        variant = Variant::callisto_3;
    else if (t == "gembloq")
        variant = Variant::gembloq;
    else if (t == "gembloq two-player")
        variant = Variant::gembloq_2;
    else if (t == "gembloq two-player four-color")
        variant = Variant::gembloq_2_4;
    else if (t == "gembloq three-player")
        variant = Variant::gembloq_3;
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
    else if (t == "callisto" || t == "ca")
        variant = Variant::callisto;
    else if (t == "callisto_2" || t == "ca2")
        variant = Variant::callisto_2;
    else if (t == "callisto_2_4" || t == "ca24")
        variant = Variant::callisto_2_4;
    else if (t == "callisto_3" || t == "ca3")
        variant = Variant::callisto_3;
    else if (t == "gembloq" || t == "g")
        variant = Variant::gembloq;
    else if (t == "gembloq_2" || t == "g2")
        variant = Variant::gembloq_2;
    else if (t == "gembloq_2_4" || t == "g24")
        variant = Variant::gembloq_2_4;
    else if (t == "gembloq_3" || t == "g3")
        variant = Variant::gembloq_3;
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
        break;
    case Variant::callisto:
        result = "Callisto";
        break;
    case Variant::callisto_2:
        result = "Callisto Two-Player";
        break;
    case Variant::callisto_2_4:
        result = "Callisto Two-Player Four-Color";
        break;
    case Variant::callisto_3:
        result = "Callisto Three-Player";
        break;
    case Variant::gembloq:
        result = "GembloQ";
        break;
    case Variant::gembloq_2:
        result = "GembloQ Two-Player";
        break;
    case Variant::gembloq_2_4:
        result = "GembloQ Two-Player Four-Color";
        break;
    case Variant::gembloq_3:
        result = "GembloQ Three-Player";
        break;
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
    case Variant::callisto:
        result = "callisto";
        break;
    case Variant::callisto_2:
        result = "callisto_2";
        break;
    case Variant::callisto_2_4:
        result = "callisto_2_4";
        break;
    case Variant::callisto_3:
        result = "callisto_3";
        break;
    case Variant::gembloq:
        result = "gembloq";
        break;
    case Variant::gembloq_2:
        result = "gembloq_2";
        break;
    case Variant::gembloq_2_4:
        result = "gembloq_2_4";
        break;
    case Variant::gembloq_3:
        result = "gembloq_3";
        break;
    }
    return result;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
