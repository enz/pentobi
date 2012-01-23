//-----------------------------------------------------------------------------
/** @file TreeTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_sgf/MissingProperty.h"
#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_test/Test.h"
#include "libpentobi_base/Tree.h"

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
    unique_ptr<Node> root = reader.get_tree_transfer_ownership();
    Tree tree(root);
    const BoardConst& board_const = tree.get_board_const();
    const Node* node = &tree.get_root();
    node = &node->get_child();
    {
        ColorMove mv = tree.get_move(*node);
        LIBBOARDGAME_CHECK(! mv.is_null());
        LIBBOARDGAME_CHECK_EQUAL(mv.color, Color(0));
        const MovePoints& points =  board_const.get_move_points(mv.move);
        LIBBOARDGAME_CHECK_EQUAL(points.size(), 5u);
        LIBBOARDGAME_CHECK(points.contains(Point("a16")));
        LIBBOARDGAME_CHECK(points.contains(Point("a17")));
        LIBBOARDGAME_CHECK(points.contains(Point("a18")));
        LIBBOARDGAME_CHECK(points.contains(Point("a19")));
        LIBBOARDGAME_CHECK(points.contains(Point("a20")));
    }
    node = &node->get_child();
    {
        ColorMove mv = tree.get_move(*node);
        LIBBOARDGAME_CHECK(! mv.is_null());
        LIBBOARDGAME_CHECK_EQUAL(mv.color, Color(1));
        const MovePoints& points =  board_const.get_move_points(mv.move);
        LIBBOARDGAME_CHECK_EQUAL(points.size(), 5u);
        LIBBOARDGAME_CHECK(points.contains(Point("s17")));
        LIBBOARDGAME_CHECK(points.contains(Point("t17")));
        LIBBOARDGAME_CHECK(points.contains(Point("t18")));
        LIBBOARDGAME_CHECK(points.contains(Point("t19")));
        LIBBOARDGAME_CHECK(points.contains(Point("t20")));
    }
    node = &node->get_child();
    {
        ColorMove mv = tree.get_move(*node);
        LIBBOARDGAME_CHECK(! mv.is_null());
        LIBBOARDGAME_CHECK_EQUAL(mv.color, Color(2));
        const MovePoints& points =  board_const.get_move_points(mv.move);
        LIBBOARDGAME_CHECK_EQUAL(points.size(), 5u);
        LIBBOARDGAME_CHECK(points.contains(Point("t1")));
        LIBBOARDGAME_CHECK(points.contains(Point("t2")));
        LIBBOARDGAME_CHECK(points.contains(Point("t3")));
        LIBBOARDGAME_CHECK(points.contains(Point("t4")));
        LIBBOARDGAME_CHECK(points.contains(Point("t5")));
    }
    node = &node->get_child();
    {
        ColorMove mv = tree.get_move(*node);
        LIBBOARDGAME_CHECK(! mv.is_null());
        LIBBOARDGAME_CHECK_EQUAL(mv.color, Color(3));
        const MovePoints& points =  board_const.get_move_points(mv.move);
        LIBBOARDGAME_CHECK_EQUAL(points.size(), 5u);
        LIBBOARDGAME_CHECK(points.contains(Point("a1")));
        LIBBOARDGAME_CHECK(points.contains(Point("b1")));
        LIBBOARDGAME_CHECK(points.contains(Point("c1")));
        LIBBOARDGAME_CHECK(points.contains(Point("d1")));
        LIBBOARDGAME_CHECK(points.contains(Point("d2")));
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
    unique_ptr<Node> root = reader.get_tree_transfer_ownership();
    Tree tree(root);
    const Node* node = &tree.get_root();
    node = &node->get_child();
    ColorMove mv = tree.get_move(*node);
    LIBBOARDGAME_CHECK(mv.move.is_pass());
}

/** Check that Tree constructor throws InvalidPropertyValue on unknown GM
    property value. */
LIBBOARDGAME_TEST_CASE(pentobi_base_tree_invalid_game)
{
    istringstream in("(;GM[1])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<Node> root = reader.get_tree_transfer_ownership();
    LIBBOARDGAME_CHECK_THROW(Tree tree(root), InvalidPropertyValue);
}

/** Check that Tree constructor throws MissingProperty on missing GM
    property. */
LIBBOARDGAME_TEST_CASE(pentobi_base_tree_missing_game_property)
{
    istringstream in("(;)");
    TreeReader reader;
    reader.read(in);
    unique_ptr<Node> root = reader.get_tree_transfer_ownership();
    LIBBOARDGAME_CHECK_THROW(Tree tree(root), MissingProperty);
}

//-----------------------------------------------------------------------------
