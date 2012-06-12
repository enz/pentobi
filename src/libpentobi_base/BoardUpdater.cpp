//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardUpdater.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardUpdater.h"

#include "BoardUtil.h"
#include "libboardgame_sgf/Util.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_sgf::InvalidTree;
using libboardgame_sgf::util::get_path_from_root;
using libpentobi_base::boardutil::get_current_position_as_setup;

//-----------------------------------------------------------------------------

namespace {

typedef ArrayList<Piece,2 * Piece::max_pieces> AllPiecesLeftList;

/** Helper function used in init_setup. */
void handle_setup_property(const Node& node, const char* id, Color c,
                           const Board& bd, Setup& setup,
                           ColorMap<AllPiecesLeftList>& pieces_left)
{
    if (! node.has_property(id))
        return;
    vector<string> values = node.get_multi_property(id);
    BOOST_FOREACH(const string& s, values)
    {
        Move mv;
        try
        {
            mv = bd.from_string(s);
        }
        catch (Exception& e)
        {
            throw InvalidTree(e.what());
        }
        Piece piece = bd.get_move_info(mv).piece;
        if (! pieces_left[c].remove(piece))
            throw InvalidTree("piece played twice");
        setup.placements[c].push_back(mv);
    }
}

/** Helper function used in init_setup. */
void handle_setup_empty(const Node& node, const Board& bd, Setup& setup,
                        ColorMap<AllPiecesLeftList>& pieces_left)
{
    if (! node.has_property("AE"))
        return;
    vector<string> values = node.get_multi_property("AE");
    BOOST_FOREACH(const string& s, values)
    {
        Move mv;
        try
        {
            mv = bd.from_string(s);
        }
        catch (Exception& e)
        {
            throw InvalidTree(e.what());
        }
        for (ColorIterator i(bd.get_nu_colors()); i; ++i)
        {
            if (setup.placements[*i].remove(mv))
            {
                Piece piece = bd.get_move_info(mv).piece;
                LIBBOARDGAME_ASSERT(! pieces_left[*i].contains(piece));
                pieces_left[*i].push_back(piece);
                break;
            }
            throw InvalidTree("invalid value for AE property");
        }
    }
}

/** Initialize the board with a new setup position.
    Class Board only supports setup positions before any moves are played. To
    support setup properties in any node, we create a new setup position from
    the current position and the setup properties from the node and initialize
    the board with it. */
void init_setup(Board& bd, const Node& node)
{
    Setup setup;
    get_current_position_as_setup(bd, setup);
    Variant variant = bd.get_variant();
    unsigned int nu_instances;
    if (variant == variant_junior)
        nu_instances = 2;
    else
        nu_instances = 1;
    ColorMap<AllPiecesLeftList> all_pieces_left;
    for (ColorIterator i(bd.get_nu_colors()); i; ++i)
        BOOST_FOREACH(Piece piece, bd.get_pieces_left(*i))
        {
            for (unsigned int j = 0; j < nu_instances; ++j)
                all_pieces_left[*i].push_back(piece);
        }
    handle_setup_property(node, "A1", Color(0), bd, setup, all_pieces_left);
    handle_setup_property(node, "A2", Color(1), bd, setup, all_pieces_left);
    handle_setup_property(node, "A3", Color(2), bd, setup, all_pieces_left);
    handle_setup_property(node, "A4", Color(3), bd, setup, all_pieces_left);
    // AB, AW are equivalent to A1, A2 but only used in games with two colors
    handle_setup_property(node, "AB", Color(0), bd, setup, all_pieces_left);
    handle_setup_property(node, "AW", Color(1), bd, setup, all_pieces_left);
    handle_setup_empty(node, bd, setup, all_pieces_left);
    Color to_play;
    if (! Tree::get_player(node, setup.to_play))
    {
        // Try to guess who should be to play based on the setup pieces.
        setup.to_play = Color(0);
        for (ColorIterator i(bd.get_nu_colors()); i; ++i)
            if (setup.placements[*i].size() < setup.placements[Color(0)].size())
            {
                setup.to_play = *i;
                break;
            }
    }
    bd.init(&setup);
}

} // namespace

//-----------------------------------------------------------------------------

void BoardUpdater::update(const Node& node)
{
    LIBBOARDGAME_ASSERT(m_tree.contains(node));
    m_bd.init();
    get_path_from_root(node, m_path);
    BOOST_FOREACH(const Node* i, m_path)
    {
        if (Tree::has_setup(*i))
            init_setup(m_bd, *i);
        ColorMove mv = m_tree.get_move(*i);
        if (mv.is_regular())
        {
            const MoveInfo& info = m_bd.get_move_info(mv.move);
            if (m_bd.get_nu_left_piece(mv.color, info.piece) == 0)
                throw InvalidTree("piece played twice");
            m_bd.play(mv);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
