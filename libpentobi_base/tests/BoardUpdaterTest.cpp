//-----------------------------------------------------------------------------
/** @file unittest/libpentobi_base/BoardUpdaterTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libpentobi_base/BoardUpdater.h"

#include "libboardgame_base/SgfUtil.h"
#include "libboardgame_base/TreeReader.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libpentobi_base;
using libboardgame_base::TreeReader;
using libboardgame_base::get_last_node;

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
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    PentobiTree tree(root);
    auto bd = make_unique<Board>(tree.get_variant());
    BoardUpdater updater;
    auto& node = get_last_node(tree.get_root());
    LIBBOARDGAME_CHECK_THROW(updater.update(*bd, tree, node), runtime_error);
}

/** Test BoardUpdater with setup properties in root node. */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_updater_setup)
{
    istringstream in("(;GM[Blokus Duo]"
                     "AB[e8,e9,f9,d10,e10][g6,f7,g7,h7,g8]"
                     "AW[i4,h5,i5,j5,i6][j7,j8,j9,k9,j10])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    PentobiTree tree(root);
    auto bd = make_unique<Board>(tree.get_variant());
    BoardUpdater updater;
    updater.update(*bd, tree, tree.get_root());
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_moves(), 0u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(0)), ScoreType(10));
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(1)), ScoreType(10));
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
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    PentobiTree tree(root);
    auto bd = make_unique<Board>(tree.get_variant());
    BoardUpdater updater;
    auto& node = get_last_node(tree.get_root());
    updater.update(*bd, tree, node);
    // BoardUpdater merges setup properties with existing position, so
    // get_nu_moves() should return the number of moves played after the setup
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_moves(), 1u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(0)), ScoreType(10));
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(1)), ScoreType(10));
}

/** Test removing a piece of Color(0) with the AE property. */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_updater_setup_empty)
{
    istringstream in("(;GM[Blokus Duo]"
                     " ;B[e8,e9,f9,d10,e10]"
                     " ;W[j7,j8,j9,k9,j10]"
                     " ;AE[e8,e9,f9,d10,e10])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    PentobiTree tree(root);
    auto bd = make_unique<Board>(tree.get_variant());
    BoardUpdater updater;
    auto& node = get_last_node(tree.get_root());
    updater.update(*bd, tree, node);
    // BoardUpdater merges setup properties with existing position, so
    // get_nu_moves() should return the number of moves played after the setup
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_moves(), 0u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(0)), ScoreType(0));
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(1)), ScoreType(5));
}

/** Test removing a piece of Color(1) with the AE property. */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_updater_setup_empty_1)
{
    istringstream in("(;GM[Blokus Duo]"
                     " ;W[e8,e9,f9,d10,e10]"
                     " ;B[j7,j8,j9,k9,j10]"
                     " ;AE[e8,e9,f9,d10,e10])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    PentobiTree tree(root);
    auto bd = make_unique<Board>(tree.get_variant());
    BoardUpdater updater;
    auto& node = get_last_node(tree.get_root());
    updater.update(*bd, tree, node);
    // BoardUpdater merges setup properties with existing position, so
    // get_nu_moves() should return the number of moves played after the setup
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_moves(), 0u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(0)), ScoreType(5));
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(1)), ScoreType(0));
}

/** Test removing a piece in a game variant with multiple instances per
    piece. */
LIBBOARDGAME_TEST_CASE(pentobi_base_board_updater_setup_empty_multi_instance)
{
    istringstream in("(;GM[Blokus Junior];B[e10];W[j5];B[f9];AE[f9][e10])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    PentobiTree tree(root);
    auto bd = make_unique<Board>(tree.get_variant());
    BoardUpdater updater;
    auto& node = get_last_node(tree.get_root());
    updater.update(*bd, tree, node);
    // BoardUpdater merges setup properties with existing position, so
    // get_nu_moves() should return the number of moves played after the setup
    LIBBOARDGAME_CHECK_EQUAL(bd->get_nu_moves(), 0u);
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(0)), ScoreType(0));
    LIBBOARDGAME_CHECK_EQUAL(bd->get_points(Color(1)), ScoreType(1));
}

//-----------------------------------------------------------------------------
