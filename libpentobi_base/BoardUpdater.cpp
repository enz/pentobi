//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardUpdater.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "BoardUpdater.h"

#include "BoardUtil.h"
#include "libboardgame_base/SgfUtil.h"

namespace libpentobi_base {

using libboardgame_base::SgfError;
using libboardgame_base::get_path_from_root;

//-----------------------------------------------------------------------------

namespace {

/** List to hold remaining pieces of a color with one entry for each instance
    of the same piece. */
using AllPiecesLeftList =
    ArrayList<Piece, PieceInfo::max_instances * Piece::max_pieces>;

/** Helper function used in init_setup. */
void handle_setup_property(const SgfNode& node, const char* id, Color c,
                           const Board& bd, Setup& setup,
                           ColorMap<AllPiecesLeftList>& pieces_left)
{
    if (! node.has_property(id))
        return;
    for (auto& s : node.get_multi_property(id))
    {
        Move mv;
        if (! bd.from_string(mv, s))
            throw SgfError("invalid move " + s);
        Piece piece = bd.get_move_piece(mv);
        if (! pieces_left[c].remove(piece))
            throw SgfError("piece played twice");
        setup.placements[c].push_back(mv);
    }
}

/** Helper function used in init_setup. */
void handle_setup_empty(const SgfNode& node, const Board& bd, Setup& setup,
                        ColorMap<AllPiecesLeftList>& pieces_left)
{
    if (! node.has_property("AE"))
        return;
    for (auto& s : node.get_multi_property("AE"))
    {
        Move mv;
        if (! bd.from_string(mv, s))
            throw SgfError("invalid move " + s);
        for (Color c : bd.get_colors())
            if (setup.placements[c].remove(mv))
            {
                Piece piece = bd.get_move_piece(mv);
                pieces_left[c].push_back(piece);
                break;
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
    for (Color c : bd.get_colors())
        for (Piece piece : bd.get_pieces_left(c))
            for (unsigned i = 0; i < bd.get_nu_piece_instances(piece); ++i)
                all_pieces_left[c].push_back(piece);
    handle_setup_property(node, "A1", Color(0), bd, setup, all_pieces_left);
    handle_setup_property(node, "A2", Color(1), bd, setup, all_pieces_left);
    handle_setup_property(node, "A3", Color(2), bd, setup, all_pieces_left);
    handle_setup_property(node, "A4", Color(3), bd, setup, all_pieces_left);
    // AB, AW are equivalent to A1, A2 but only used in games with two colors
    handle_setup_property(node, "AB", Color(0), bd, setup, all_pieces_left);
    handle_setup_property(node, "AW", Color(1), bd, setup, all_pieces_left);
    handle_setup_empty(node, bd, setup, all_pieces_left);
    Color to_play;
    if (! libpentobi_base::get_player(node, bd.get_nu_colors(),
                                                 setup.to_play))
    {
        // Try to guess who should be to play based on the setup pieces.
        setup.to_play = Color(0);
        for (Color c : bd.get_colors())
            if (setup.placements[c].size() < setup.placements[Color(0)].size())
            {
                setup.to_play = c;
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
        if (libpentobi_base::has_setup(*i))
            init_setup(bd, *i);
        auto mv = tree.get_move(*i);
        if (! mv.is_null())
        {
            if (! bd.is_piece_left(mv.color, bd.get_move_piece(mv.move)))
                throw SgfError("piece played twice");
            bd.play(mv);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
