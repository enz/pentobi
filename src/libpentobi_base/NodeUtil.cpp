//-----------------------------------------------------------------------------
/** @file libpentobi_base/NodeUtil.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "NodeUtil.h"

#include "libboardgame_util/StringUtil.h"

namespace libpentobi_base {
namespace node_util {

using libboardgame_sgf::InvalidPropertyValue;
using libboardgame_sgf::InvalidTree;
using libboardgame_util::split;
using libboardgame_util::trim;

//-----------------------------------------------------------------------------

bool has_move(const SgfNode& node, Variant variant)
{
    // See also comment in get_move()
    switch (get_nu_colors(variant))
    {
    case 2:
        for (auto& prop : node.get_properties())
        {
            auto& id = prop.id;
            if (id == "B" || id == "W" || id == "1" || id == "2"
                    || id == "BLUE" || id == "GREEN")
                return true;
        }
        break;
    case 3:
        for (auto& prop : node.get_properties())
        {
            auto& id = prop.id;
            if (id == "1" || id == "2" || id == "3" || id == "BLUE"
                    || id == "YELLOW" || id == "RED")
                return true;
        }
        break;
    case 4:
        for (auto& prop : node.get_properties())
        {
            auto& id = prop.id;
            if (id == "1" || id == "2" || id == "3" || id == "4"
                    || id == "BLUE" || id == "YELLOW" || id == "RED"
                     || id == "GREEN")
                return true;
        }
        break;
    default:
        LIBBOARDGAME_ASSERT(false);
    }
    return false;
}

bool get_move(const SgfNode& node, Variant variant, Color& c,
              MovePoints& points)
{
    auto nu_colors = get_nu_colors(variant);
    string id;
    // Pentobi 0.1 used BLUE/YELLOW/RED/GREEN instead of 1/2/3/4 as suggested
    // by SGF FF[5]. Pentobi 12.0 erroneosly used 1/2 for two-player Callisto
    // instead of B/W. We still want to be able to read files written by older
    // versions. They will be converted to the current format by
    // PentobiTreeWriter.
    if (nu_colors == 2)
    {
        if (node.has_property("B"))
        {
            id = "B";
            c = Color(0);
        }
        else if (node.has_property("W"))
        {
            id = "W";
            c = Color(1);
        }
        else if (node.has_property("1"))
        {
            id = "1";
            c = Color(0);
        }
        else if (node.has_property("2"))
        {
            id = "2";
            c = Color(1);
        }
        else if (node.has_property("BLUE"))
        {
            id = "BLUE";
            c = Color(0);
        }
        else if (node.has_property("GREEN"))
        {
            id = "GREEN";
            c = Color(1);
        }
    }
    else
    {
        if (node.has_property("1"))
        {
            id = "1";
            c = Color(0);
        }
        else if (node.has_property("2"))
        {
            id = "2";
            c = Color(1);
        }
        else if (node.has_property("3"))
        {
            id = "3";
            c = Color(2);
        }
        else if (node.has_property("4"))
        {
            id = "4";
            c = Color(3);
        }
        else if (node.has_property("BLUE"))
        {
            id = "BLUE";
            c = Color(0);
        }
        else if (node.has_property("YELLOW"))
        {
            id = "YELLOW";
            c = Color(1);
        }
        else if (node.has_property("RED"))
        {
            id = "RED";
            c = Color(2);
        }
        else if (node.has_property("GREEN"))
        {
            id = "GREEN";
            c = Color(3);
        }
    }
    if (id.empty() || c.to_int() >= nu_colors)
        return false;
    // Note: we still support having the points of a move in a list of point
    // values instead of a single value as used by Pentobi <= 0.2, but it
    // is deprecated
    points.clear();
    auto& geo = get_geometry(variant);
    bool is_nexos = (get_board_type(variant) == BoardType::nexos);
    for (auto& s : node.get_multi_property(id))
    {
        if (trim(s).empty())
            continue;
        vector<string> v = split(s, ',');
        for (const auto& p_str : v)
        {
            Point p;
            if (! geo.from_string(p_str, p))
                throw InvalidPropertyValue(id, p_str);
            if (is_nexos)
            {
                auto point_type = geo.get_point_type(p);
                if (point_type != 1 && point_type != 2)
                    // Silently discard points that are not line segments, such
                    // files were written by some (unreleased) versions of
                    // Pentobi.
                    continue;
            }
            if (points.size() == points.max_size)
                throw InvalidPropertyValue(id, p_str);
            points.push_back(p);
        }
    }
    return true;
}

bool get_player(const SgfNode& node, Color& c)
{
    if (! node.has_property("PL"))
        return false;
    string value = node.get_property("PL");
    if (value == "B" || value == "1")
        c = Color(0);
    else if (value == "W" || value == "2")
        c = Color(1);
    else if (value == "3")
        c = Color(2);
    else if (value == "4")
        c = Color(3);
    else
        throw InvalidTree("invalid value for PL property");
    return true;
}

bool has_setup(const SgfNode& node)
{
    for (auto& i : node.get_properties())
        if (i.id == "AB" || i.id == "AW" || i.id == "A1" || i.id == "A2"
                || i.id == "A3" || i.id == "A4" || i.id == "AE")
            return true;
    return false;
}

//-----------------------------------------------------------------------------

} // namespace node_util
} // namespace libpentobi_base
