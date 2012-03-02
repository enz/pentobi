//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardUpdater.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardUpdater.h"

#include "libboardgame_sgf/Util.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_sgf::util::get_path_from_root;

//-----------------------------------------------------------------------------

namespace {

void check_setup_property(const Node& node, const string& id)
{
    if (node.has_property(id))
        throw Exception("Setup properties not yet supported");
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
        // Setup properties are not defined or used in the Pentobi SGF format
        // at the moment, but we don't want to silently ignore them should
        // they be used in the future because then we would misinterpret the
        // board state.
        check_setup_property(*i, "AB");
        check_setup_property(*i, "AW");
        check_setup_property(*i, "A1");
        check_setup_property(*i, "A2");
        check_setup_property(*i, "A3");
        check_setup_property(*i, "A4");
        check_setup_property(*i, "AE");
        ColorMove mv = m_tree.get_move(*i);
        if (! mv.is_null())
        {
            if (m_bd.get_nu_moves() >= Board::max_game_moves)
                throw Exception("too many moves");
            const MoveInfo& info = m_bd.get_move_info(mv.move);
            if (! m_bd.get_pieces_left(mv.color).contains(info.piece))
                throw Exception("piece played twice");
            m_bd.play(mv);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
