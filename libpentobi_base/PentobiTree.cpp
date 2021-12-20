//-----------------------------------------------------------------------------
/** @file libpentobi_base/PentobiTree.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "PentobiTree.h"

#include "BoardUpdater.h"
#include "BoardUtil.h"

namespace libpentobi_base {

using libboardgame_base::InvalidProperty;
using libboardgame_base::SgfError;

//-----------------------------------------------------------------------------

PentobiTree::PentobiTree(Variant variant)
{
    init_variant(variant);
}

PentobiTree::PentobiTree(unique_ptr<SgfNode>& root)
{
    PentobiTree::init(root);
}

const SgfNode& PentobiTree::add_setup(const SgfNode& node, Color c, Move mv)
{
    const SgfNode* result;
    if (has_move(node))
        result = &create_new_child(node);
    else
        result = &node;
    auto add_empty = get_setup_property(*result, "AE");
    if (add_empty.remove(mv))
        set_setup_property(*result, "AE", add_empty);
    auto id = get_setup_prop_id(c);
    auto add_color = get_setup_property(*result, id);
    if (! add_color.contains(mv)
            && add_color.size() < Setup::PlacementList::max_size)
    {
        add_color.push_back(mv);
        set_setup_property(*result, id, add_color);
    }
    return *result;
}

const SgfNode* PentobiTree::find_child_with_move(const SgfNode& node,
                                          ColorMove mv) const
{
    for (auto& i : node.get_children())
        if (get_move(i) == mv)
            return &i;
    return nullptr;
}

ColorMove PentobiTree::get_move(const SgfNode& node) const
{
    Color c;
    MovePoints points;
    if (! libpentobi_base::get_move(node, m_variant, c, points))
        return ColorMove::null();
    Move mv;
    if (! m_bc->find_move(points, mv))
        throw SgfError("Tree contains illegal move");
    return {c, mv};
}

const SgfNode* PentobiTree::get_node_before_move_number(
        unsigned move_number) const
{
    auto node = &get_root();
    unsigned n = 0;
    while (node->has_children())
    {
        auto& child = node->get_first_child();
        if (has_move(child) && n++ == move_number)
                return node;
        node = &child;
    }
    return nullptr;
}

string PentobiTree::get_player_name(Color c) const
{
    string name;
    auto& root = get_root();
    if (get_nu_players(m_variant) == 2)
    {
        if (c == Color(0) || c == Color(2))
            name = root.get_property("PB", "");
        else if (c == Color(1) || c == Color(2))
            name = root.get_property("PW", "");
    }
    else
    {
        if (c == Color(0))
            name = root.get_property("P1", "");
        else if (c == Color(1))
            name = root.get_property("P2", "");
        else if (c == Color(2))
            name = root.get_property("P3", "");
        else if (c == Color(3))
            name = root.get_property("P4", "");
    }
    return name;
}

Setup::PlacementList PentobiTree::get_setup_property(const SgfNode& node,
                                                     const char* id) const
{
    Setup::PlacementList result;
    if (node.has_property(id))
        for (auto& s : node.get_multi_property(id))
        {
            if (result.size() == Setup::PlacementList::max_size)
                throw InvalidProperty(id, s);
            Move mv;
            if (! m_bc->from_string(mv, s))
                throw InvalidProperty(id, s);
            result.push_back(mv);
        }
    return result;
}

Variant PentobiTree::get_variant(const SgfNode& root)
{
    string game = root.get_property("GM");
    Variant variant;
    if (! parse_variant(game, variant))
        throw InvalidProperty("GM", game);
    return variant;
}

void PentobiTree::init(unique_ptr<SgfNode>& root)
{
    Variant variant = get_variant(*root);
    SgfTree::init(root);
    m_variant = variant;
    init_board_const(variant);
}

void PentobiTree::init_board_const(Variant variant)
{
    m_bc = &BoardConst::get(variant);
}

void PentobiTree::init_variant(Variant variant)
{
    SgfTree::init();
    m_variant = variant;
    set_game_property();
    init_board_const(variant);
    clear_modified();
}

void PentobiTree::keep_only_subtree(const SgfNode& node)
{
    LIBBOARDGAME_ASSERT(contains(node));
    if (&node == &get_root())
        return;
    string charset = get_root().get_property("CA", "");
    string application = get_root().get_property("AP", "");
    bool create_new_setup = has_move(node);
    if (! create_new_setup)
    {
        auto current = node.get_parent_or_null();
        while (current != nullptr)
        {
            if (has_move(*current) || has_setup(*current))
            {
                create_new_setup = true;
                break;
            }
            current = current->get_parent_or_null();
        }
    }
    if (create_new_setup)
    {
        auto bd = make_unique<Board>(m_variant);
        BoardUpdater updater;
        updater.update(*bd, *this, node);
        Setup setup;
        get_current_position_as_setup(*bd, setup);
        set_setup(node, setup);
    }
    make_root(node);
    if (! application.empty())
    {
        set_property(node, "AP", application);
        move_property_to_front(node, "AP");
    }
    if (! charset.empty())
    {
        set_property(node, "CA", charset);
        move_property_to_front(node, "CA");
    }
    set_game_property();
}

bool PentobiTree::remove_player(const SgfNode& node)
{
    return remove_property(node, "PL");
}

const SgfNode& PentobiTree::remove_setup(const SgfNode& node, Color c,
                                            Move mv)
{
    const SgfNode* result;
    if (has_move(node))
        result = &create_new_child(node);
    else
        result = &node;
    auto id = get_setup_prop_id(c);
    auto add_color = get_setup_property(*result, id);
    if (add_color.remove(mv))
        set_setup_property(*result, id, add_color);
    else
    {
        auto add_empty = get_setup_property(*result, "AE");
        if (! add_empty.contains(mv)
                && add_empty.size() < Setup::PlacementList::max_size)
        {
            add_empty.push_back(mv);
            set_setup_property(*result, "AE", add_empty);
        }
    }
    return *result;
}

void PentobiTree::set_game_property()
{
    auto& root = get_root();
    set_property(root, "GM", to_string(m_variant));
    move_property_to_front(root, "GM");
}

void PentobiTree::set_move(const SgfNode& node, Color c, Move mv)
{
    LIBBOARDGAME_ASSERT(! mv.is_null());
    auto id = get_color(c);
    set_property(node, id, m_bc->to_string(mv, false));
}

void PentobiTree::set_player(const SgfNode& node, Color c)
{
    set_property(node, "PL", get_color(c));
}

void PentobiTree::set_player_name(Color c, const string& name)
{
    auto& root = get_root();
    if (get_nu_players(m_variant) == 2)
    {
        if (c == Color(0) || c == Color(2))
            set_property_remove_empty(root, "PB", name);
        else if (c == Color(1) || c == Color(3))
            set_property_remove_empty(root, "PW", name);
    }
    else
    {
        if (c == Color(0))
            set_property_remove_empty(root, "P1", name);
        else if (c == Color(1))
            set_property_remove_empty(root, "P2", name);
        else if (c == Color(2))
            set_property_remove_empty(root, "P3", name);
        else if (c == Color(3))
            set_property_remove_empty(root, "P4", name);
    }
}

void PentobiTree::set_result(const SgfNode& node, int score)
{
    if (score > 0)
    {
        ostringstream s;
        s << "B+" << score;
        set_property(node, "RE", s.str());
    }
    else if (score < 0)
    {
        ostringstream s;
        s << "W+" << (-score);
        set_property(node, "RE", s.str());
    }
    else
        set_property(node, "RE", "0");
}

void PentobiTree::set_setup(const SgfNode& node, const Setup& setup)
{
    auto nu_colors = get_nu_colors(m_variant);
    LIBBOARDGAME_ASSERT(nu_colors >= 2 && nu_colors <= 4);
    remove_property(node, "B");
    remove_property(node, "W");
    remove_property(node, "1");
    remove_property(node, "2");
    remove_property(node, "3");
    remove_property(node, "4");
    remove_property(node, "AB");
    remove_property(node, "AW");
    remove_property(node, "A1");
    remove_property(node, "A2");
    remove_property(node, "A3");
    remove_property(node, "A4");
    remove_property(node, "AE");
    if (nu_colors == 2)
    {
        set_setup_property(node, "AB", setup.placements[Color(0)]);
        set_setup_property(node, "AW", setup.placements[Color(1)]);
    }
    else
    {
        set_setup_property(node, "A1", setup.placements[Color(0)]);
        set_setup_property(node, "A2", setup.placements[Color(1)]);
        set_setup_property(node, "A3", setup.placements[Color(2)]);
        if (nu_colors > 3)
            set_setup_property(node, "A4", setup.placements[Color(3)]);
    }
    set_player(node, setup.to_play);
}

void PentobiTree::set_setup_property(const SgfNode& node, const char* id,
                                     const Setup::PlacementList& placements)
{
    if (placements.empty())
    {
        remove_property(node, id);
        return;
    }
    vector<string> values;
    values.reserve(placements.size());
    for (Move mv : placements)
        values.push_back(m_bc->to_string(mv, false));
    set_property(node, id, values);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
