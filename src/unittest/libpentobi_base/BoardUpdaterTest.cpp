//-----------------------------------------------------------------------------
/** @file unittest/libpentobi_base/BoardUpdaterTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
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
    unique_ptr<Board> bd(new Board(tree.get_variant()));
    BoardUpdater updater;
    auto& node = get_last_node(tree.get_root());
    LIBBOARDGAME_CHECK_THROW(updater.update(*bd, tree, node), Exception);
}

/** Test BoardUpdater with setup properties in root node. */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_updater_setup)
{
    istringstream in("(;GM[Blokus Duo]"
                     "AB[e8,e9,f9,d10,e10][g6,f7,g7,h7,g8]"
                     "AW[i4,h5,i5,j5,i6][j7,j8,j9,k9,j10])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<Node> root = reader.get_tree_transfer_ownership();
    Tree tree(root);
    unique_ptr<Board> bd(new Board(tree.get_variant()));
    BoardUpdater updater;
    updater.update(*bd, tree, tree.get_root());
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_moves(), 0u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(0)), 10u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(1)), 10u);
}

/** Test BoardUpdater with setup properties in an inner node. */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_updater_setup_inner_node)
{
    istringstream in("(;GM[Blokus Duo]"
                     " ;B[e8,e9,f9,d10,e10]"
                     " ;AB[g6,f7,g7,h7,g8]AW[i4,h5,i5,j5,i6]"
                     " ;W[j7,j8,j9,k9,j10])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<Node> root = reader.get_tree_transfer_ownership();
    Tree tree(root);
    unique_ptr<Board> bd(new Board(tree.get_variant()));
    BoardUpdater updater;
    auto& node = get_last_node(tree.get_root());
    updater.update(*bd, tree, node);
    // BoardUpdater merges setup properties with existing position, so
    // get_nu_moves() should return the number of moves played after the setup
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_moves(), 1u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(0)), 10u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(1)), 10u);
}

/** Test removing a piece with the AE property. */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_updater_setup_empty)
{
    istringstream in("(;GM[Blokus Duo]"
                     " ;B[e8,e9,f9,d10,e10]"
                     " ;W[j7,j8,j9,k9,j10]"
                     " ;AE[e8,e9,f9,d10,e10])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<Node> root = reader.get_tree_transfer_ownership();
    Tree tree(root);
    unique_ptr<Board> bd(new Board(tree.get_variant()));
    BoardUpdater updater;
    auto& node = get_last_node(tree.get_root());
    updater.update(*bd, tree, node);
    // BoardUpdater merges setup properties with existing position, so
    // get_nu_moves() should return the number of moves played after the setup
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_moves(), 0u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(0)), 0u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(1)), 5u);
}

//-----------------------------------------------------------------------------
