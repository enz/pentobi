//-----------------------------------------------------------------------------
/** @file libpentobi_base/Tree.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Tree.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "libboardgame_sgf/Util.h"
#include "libboardgame_util/StringUtil.h"

namespace libpentobi_base {

using boost::trim_copy;
using boost::algorithm::to_lower_copy;
using libboardgame_sgf::ChildIterator;
using libboardgame_sgf::InvalidPropertyValue;
using libboardgame_sgf::util::get_go_point_property_value;
using libboardgame_sgf::util::parse_go_move_property_value;
using libboardgame_sgf::util::parse_go_point_property_value;
using libboardgame_util::string_util::to_string;

//-----------------------------------------------------------------------------

Tree::Tree()
{
    init();
}

const Node* Tree::find_child_with_move(const Node& node, ColorMove mv) const
{
    for (ChildIterator i(node); i; ++i)
        if (get_move(*i) == mv)
            return &(*i);
    return 0;
}

ColorMove Tree::get_move(const Node& node) const
{
    Color c = Color(0); // Initialize to avoid compiler warning
    string id;
    vector<string> values;
    if (m_game_variant == game_variant_duo)
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
    }
    else
    {
        if (node.has_property("BLUE"))
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
        return ColorMove::null();
    values = node.get_multi_property(id);
    if (values.size() == 1 && values[0].empty())
        return ColorMove(c, Move::pass());
    MovePoints points;
    BOOST_FOREACH(const string& s, values)
    {
        if (trim_copy(s).empty())
            throw InvalidPropertyValue("Move contains pass point");
        points.push_back(Point::from_string(s));
    }
    Move mv;
    if (! m_board_const->find_move(points, mv))
        throw InvalidPropertyValue(id,
                                   format("Illegal move %1%")
                                   % to_string(points));
    return ColorMove(c, mv);
}

const Node* Tree::get_node_with_move_number(unsigned int move_number) const
{
    const Node* node = &get_root();
    unsigned int n = 0;
    while (true)
    {
        if (! get_move(*node).is_null())
            if (++n == move_number)
                return node;
        if (! node->has_children())
            break;
        node = &node->get_child();
    }
    return 0;
}

void Tree::init(GameVariant game_variant)
{
    libboardgame_sgf::Tree::init();
    m_game_variant = game_variant;
    const Node& root = get_root();
    switch (game_variant)
    {
    case game_variant_classic:
        m_sz = 20;
        set_property(root, "GM", "Blokus");
        break;
    case game_variant_classic_2:
        m_sz = 20;
        set_property(root, "GM", "Blokus Two-Player");
        break;
    case game_variant_duo:
        m_sz = 14;
        set_property(root, "GM", "Blokus Duo");
        break;
    }
    m_board_const = &BoardConst::get(m_sz);
    clear_modified();
}

void Tree::init(unique_ptr<Node>& root)
{
    libboardgame_sgf::Tree::init(root);
    string game = get_root().get_property("GM");
    string s = to_lower_copy(trim_copy(game));
    if (s == "blokus")
    {
        m_sz = 20;
        m_game_variant = game_variant_classic;
    }
    else if (s == "blokus two-player")
    {
        m_sz = 20;
        m_game_variant = game_variant_classic_2;
    }
    else if (s == "blokus duo")
    {
        m_sz = 14;
        m_game_variant = game_variant_duo;
    }
    else
        throw InvalidPropertyValue("GM", game);
    m_board_const = &BoardConst::get(m_sz);
}

void Tree::set_move(const Node& node, Color c, Move mv)
{
    string id;
    if (m_game_variant == game_variant_duo)
        id = (c == Color(0) ? "B" : "W");
    else
    {
        if (c == Color(0))
            id = "BLUE";
        else if (c == Color(1))
            id = "YELLOW";
        else if (c == Color(2))
            id = "RED";
        else if (c == Color(3))
            id = "GREEN";
    }
    vector<string> values;
    if (! mv.is_pass())
    {
        BOOST_FOREACH(Point p, m_board_const->get_move_points(mv))
            values.push_back(p.to_string());
    }
    else
        values.push_back("");
    set_property(node, id, values);
}

void Tree::set_result(const Node& node, int score)
{
    if (score > 0)
        set_property(node, "RE", format("B+%i") % score);
    else if (score < 0)
        set_property(node, "RE", format("W+%i") % -score);
    else
        set_property(node, "RE", "0");
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
