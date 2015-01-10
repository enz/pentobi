//-----------------------------------------------------------------------------
/** @file unittest/libpentobi_base/TreeTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_sgf/MissingProperty.h"
#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_test/Test.h"
#include "libpentobi_base/PentobiTree.h"

using namespace std;
using namespace libpentobi_base;
using libboardgame_sgf::InvalidPropertyValue;
using libboardgame_sgf::MissingProperty;
using libboardgame_sgf::TreeReader;

//-----------------------------------------------------------------------------

/** Check backwards compatibility to move properties used in Pentobi 0.1.
    Pentobi 0.1 used the property id's BLUE,YELLOW,RED,GREEN in four-player
    game variants instead of 1,2,3,4. (It also used point lists instead of
    single-value move properties. */
LIBBOARDGAME_TEST_CASE(pentobi_base_tree_backward_compatibility_0_1)
{
    istringstream in("(;GM[Blokus Two-Player];BLUE[a16][a17][a18][a19][a20]"
                     ";YELLOW[s17][t17][t18][t19][t20];RED[t1][t2][t3][t4][t5]"
                     ";GREEN[a1][b1][c1][d1][d2])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    PentobiTree tree(root);
    auto& bc = tree.get_board_const();
    auto& geo = bc.get_geometry();
    auto node = &tree.get_root();
    node = &node->get_child();
    {
        auto mv = tree.get_move(*node);
        LIBBOARDGAME_CHECK(! mv.is_null());
        LIBBOARDGAME_CHECK_EQUAL(mv.color, Color(0));
        auto& info = bc.get_move_info(mv.move);
        LIBBOARDGAME_CHECK_EQUAL(info.size(), 5u);
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(0, 4)));
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(0, 3)));
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(0, 2)));
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(0, 1)));
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(0, 0)));
    }
    node = &node->get_child();
    {
        auto mv = tree.get_move(*node);
        LIBBOARDGAME_CHECK(! mv.is_null());
        LIBBOARDGAME_CHECK_EQUAL(mv.color, Color(1));
        auto& info = bc.get_move_info(mv.move);
        LIBBOARDGAME_CHECK_EQUAL(info.size(), 5u);
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(18, 3)));
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(19, 3)));
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(19, 2)));
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(19, 1)));
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(19, 0)));
    }
    node = &node->get_child();
    {
        auto mv = tree.get_move(*node);
        LIBBOARDGAME_CHECK(! mv.is_null());
        LIBBOARDGAME_CHECK_EQUAL(mv.color, Color(2));
        auto& info = bc.get_move_info(mv.move);
        LIBBOARDGAME_CHECK_EQUAL(info.size(), 5u);
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(19, 19)));
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(19, 18)));
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(19, 17)));
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(19, 16)));
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(19, 15)));
    }
    node = &node->get_child();
    {
        auto mv = tree.get_move(*node);
        LIBBOARDGAME_CHECK(! mv.is_null());
        LIBBOARDGAME_CHECK_EQUAL(mv.color, Color(3));
        auto& info = bc.get_move_info(mv.move);
        LIBBOARDGAME_CHECK_EQUAL(info.size(), 5u);
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(0, 19)));
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(1, 19)));
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(2, 19)));
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(3, 19)));
        LIBBOARDGAME_CHECK(info.contains(geo.get_point(3, 18)));
    }
}

/** Check that Tree::get_move() can handle pass moves.
    The current Blokus SGF documentation included in Pentobi does not specify
    if pass moves are allowed, but they may be used in the future and early
    (unreleased) versions of Pentobi use them. */
LIBBOARDGAME_TEST_CASE(pentobi_base_tree_get_move_pass)
{
    istringstream in("(;GM[Blokus Duo];B[])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    PentobiTree tree(root);
    auto node = &tree.get_root();
    node = &node->get_child();
    auto mv = tree.get_move(*node);
    LIBBOARDGAME_CHECK(mv.move.is_pass());
}

/** Check that Tree constructor throws InvalidPropertyValue on unknown GM
    property value. */
LIBBOARDGAME_TEST_CASE(pentobi_base_tree_invalid_game)
{
    istringstream in("(;GM[1])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    LIBBOARDGAME_CHECK_THROW(PentobiTree tree(root), InvalidPropertyValue);
}

/** Check that Tree constructor throws MissingProperty on missing GM
    property. */
LIBBOARDGAME_TEST_CASE(pentobi_base_tree_missing_game_property)
{
    istringstream in("(;)");
    TreeReader reader;
    reader.read(in);
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    LIBBOARDGAME_CHECK_THROW(PentobiTree tree(root), MissingProperty);
}

//-----------------------------------------------------------------------------
