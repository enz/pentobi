//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardUpdater.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardUpdater.h"

#include "BoardUtil.h"
#include "NodeUtil.h"
#include "libboardgame_sgf/SgfUtil.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_sgf::InvalidTree;
using libboardgame_sgf::util::get_path_from_root;
using libpentobi_base::boardutil::get_current_position_as_setup;

//-----------------------------------------------------------------------------

namespace {

typedef ArrayList<Piece,2 * Piece::max_pieces> AllPiecesLeftList;

/** Helper function used in init_setup. */
void handle_setup_property(const SgfNode& node, const char* id, Color c,
                           const Board& bd, Setup& setup,
                           ColorMap<AllPiecesLeftList>& pieces_left)
{
    if (! node.has_property(id))
        return;
    vector<string> values = node.get_multi_property(id);
    for (const string& s : values)
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
        Piece piece = bd.get_move_info(mv).get_piece();
        if (! pieces_left[c].remove(piece))
            throw InvalidTree("piece played twice");
        setup.placements[c].push_back(mv);
    }
}

/** Helper function used in init_setup. */
void handle_setup_empty(const SgfNode& node, const Board& bd, Setup& setup,
                        ColorMap<AllPiecesLeftList>& pieces_left)
{
    if (! node.has_property("AE"))
        return;
    vector<string> values = node.get_multi_property("AE");
    for (const string& s : values)
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
                Piece piece = bd.get_move_info(mv).get_piece();
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
void init_setup(Board& bd, const SgfNode& node)
{
    Setup setup;
    get_current_position_as_setup(bd, setup);
    ColorMap<AllPiecesLeftList> all_pieces_left;
    for (ColorIterator i(bd.get_nu_colors()); i; ++i)
        for (Piece piece : bd.get_pieces_left(*i))
            for (unsigned j = 0; j < bd.get_nu_piece_instances(); ++j)
                all_pieces_left[*i].push_back(piece);
    handle_setup_property(node, "A1", Color(0), bd, setup, all_pieces_left);
    handle_setup_property(node, "A2", Color(1), bd, setup, all_pieces_left);
    handle_setup_property(node, "A3", Color(2), bd, setup, all_pieces_left);
    handle_setup_property(node, "A4", Color(3), bd, setup, all_pieces_left);
    // AB, AW are equivalent to A1, A2 but only used in games with two colors
    handle_setup_property(node, "AB", Color(0), bd, setup, all_pieces_left);
    handle_setup_property(node, "AW", Color(1), bd, setup, all_pieces_left);
    handle_setup_empty(node, bd, setup, all_pieces_left);
    Color to_play;
    if (! libpentobi_base::node_util::get_player(node, setup.to_play))
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

void BoardUpdater::update(Board& bd, const PentobiTree& tree,
                          const SgfNode& node)
{
    LIBBOARDGAME_ASSERT(tree.contains(node));
    bd.init();
    get_path_from_root(node, m_path);
    for (const auto i : m_path)
    {
        if (libpentobi_base::node_util::has_setup(*i))
            init_setup(bd, *i);
        auto mv = tree.get_move(*i);
        if (! mv.is_null())
        {
            auto& info = bd.get_move_info(mv.move);
            if (! bd.is_piece_left(mv.color, info.get_piece()))
                throw InvalidTree("piece played twice");
            bd.play(mv);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
