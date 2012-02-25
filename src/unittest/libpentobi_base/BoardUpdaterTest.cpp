//-----------------------------------------------------------------------------
/** @file unittest/libpentobi_base/BoardUpdaterTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libpentobi_base/BoardUpdater.h"

#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_sgf/Util.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libpentobi_base;
using libboardgame_sgf::TreeReader;
using libboardgame_sgf::util::get_last_node;

//-----------------------------------------------------------------------------

/** Test that BoardUpdater throws an exception if a piece is played twice.
    A tree from a file written by another application could contain move
    sequences where a piece is played twice. This could break assumptions
    about the maximum number of moves in a game at some places in Pentobi's
    code, so BoardUpdater should detect this and throw an exception. */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_updater_piece_played_twice)
{
    istringstream in("(;GM[Blokus];1[a1];1[a3])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<Node> root = reader.get_tree_transfer_ownership();
    Tree tree(root);
    Board bd(tree.get_game_variant());
    BoardUpdater updater(tree, bd);
    const Node& node = get_last_node(tree.get_root());
    LIBBOARDGAME_CHECK_THROW(updater.update(node), Exception);
}

//-----------------------------------------------------------------------------
