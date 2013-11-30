//-----------------------------------------------------------------------------
/** @file libpentobi_base/NodeUtil.cpp
    @author Markus Enzenberger <enz@users.sourceforge.net>
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
using libboardgame_sgf::PropertyIterator;
using libboardgame_util::split;
using libboardgame_util::trim;

//-----------------------------------------------------------------------------

bool get_move(const Node& node, Variant variant, Color& c, MovePoints& points)
{
    string id;
    if (variant == Variant::duo || variant == Variant::junior)
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
    }
    else
    {
        // Properties BLUE/YELLOW/RED/GREEN were used by Pentobi 0.1
        // Newer versions of Pentobi use 1/2/3/4 as suggested by SGF FF[5]
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
    if (id.empty())
        return false;
    vector<string> values;
    values = node.get_multi_property(id);
    // Note: we still support having the points of a move in a list of point
    // values instead of a single value as used by Pentobi <= 0.2, but it
    // is deprecated
    points.clear();
    for (const auto& s : values)
    {
        if (trim(s).empty())
            continue;
        vector<string> v = split(s, ',');
        for (const auto& p_str : v)
        {
            Point p;
            try
            {
                p = Point::from_string(p_str);
            }
            catch (const Point::InvalidString&)
            {
                throw InvalidPropertyValue(id, p_str);
            }
            points.push_back(p);
        }
    }
    return true;
}

bool get_player(const Node& node, Color& c)
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

bool has_setup(const Node& node)
{
    for (PropertyIterator i(node); i; ++i)
        if (i->id == "AB" || i->id == "AW" || i->id == "A1" || i->id == "A2"
            || i->id == "A3" || i->id == "A4" || i->id == "AE")
            return true;
    return false;
}

//-----------------------------------------------------------------------------

} // namespace node_util
} // namespace libpentobi_base
