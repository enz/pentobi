//-----------------------------------------------------------------------------
/** @file unittest/libpentobi_base/PentobiTreeTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_test/Test.h"
#include "libpentobi_base/PentobiTree.h"

using namespace std;
using namespace libpentobi_base;
using libboardgame_sgf::InvalidProperty;
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
        LIBBOARDGAME_CHECK_EQUAL(mv.color.to_int(), 0u);
        auto points = bc.get_move_points(mv.move);
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(0, 4)));
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(0, 3)));
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(0, 2)));
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(0, 1)));
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(0, 0)));
    }
    node = &node->get_child();
    {
        auto mv = tree.get_move(*node);
        LIBBOARDGAME_CHECK(! mv.is_null());
        LIBBOARDGAME_CHECK_EQUAL(mv.color.to_int(), 1u);
        auto points = bc.get_move_points(mv.move);
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(18, 3)));
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(19, 3)));
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(19, 2)));
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(19, 1)));
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(19, 0)));
    }
    node = &node->get_child();
    {
        auto mv = tree.get_move(*node);
        LIBBOARDGAME_CHECK(! mv.is_null());
        LIBBOARDGAME_CHECK_EQUAL(mv.color.to_int(), 2u);
        auto points = bc.get_move_points(mv.move);
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(19, 19)));
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(19, 18)));
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(19, 17)));
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(19, 16)));
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(19, 15)));
    }
    node = &node->get_child();
    {
        auto mv = tree.get_move(*node);
        LIBBOARDGAME_CHECK(! mv.is_null());
        LIBBOARDGAME_CHECK_EQUAL(mv.color.to_int(), 3u);
        auto points = bc.get_move_points(mv.move);
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(0, 19)));
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(1, 19)));
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(2, 19)));
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(3, 19)));
        LIBBOARDGAME_CHECK(points.contains(geo.get_point(3, 18)));
    }
}

/** Check that Tree constructor throws InvalidProperty on unknown GM property
    value. */
LIBBOARDGAME_TEST_CASE(pentobi_base_tree_invalid_game)
{
    istringstream in("(;GM[1])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    LIBBOARDGAME_CHECK_THROW(PentobiTree tree(root), InvalidProperty);
}

/** Check that keep_only_subtree() works in Callisto.
    Regression test for a bug in Pentobi 12.0 */
LIBBOARDGAME_TEST_CASE(pentobi_base_tree_keep_only_subtree_callisto)
{

    istringstream in("(;GM[Callisto]"
                     ";1[h12]"
                     ";2[m9]"
                     ";3[l8]"
                     ";4[i13]"
                     ";1[i8]"
                     ";2[p14])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    PentobiTree tree(root);
    auto node = &tree.get_root();
    node = &node->get_first_child();
    node = &node->get_first_child();
    node = &node->get_first_child();
    node = &node->get_first_child();
    node = &node->get_first_child();
    LIBBOARDGAME_CHECK(! tree.is_modified());
    tree.keep_only_subtree(*node);
    LIBBOARDGAME_CHECK(tree.is_modified());

    node = &tree.get_root();
    LIBBOARDGAME_CHECK(node->has_single_child());
    {
        auto& values = node->get_multi_property("A1");
        LIBBOARDGAME_CHECK_EQUAL(values.size(), 2u);
        auto begin = values.begin();
        auto end = values.end();
        LIBBOARDGAME_CHECK(find(begin, end, "h12") != end);
        LIBBOARDGAME_CHECK(find(begin, end, "i8") != end);
    }
    {
        auto& values = node->get_multi_property("A2");
        LIBBOARDGAME_CHECK_EQUAL(values.size(), 1u);
        LIBBOARDGAME_CHECK_EQUAL(values[0], "m9");
    }
    {
        auto& values = node->get_multi_property("A3");
        LIBBOARDGAME_CHECK_EQUAL(values.size(), 1u);
        LIBBOARDGAME_CHECK_EQUAL(values[0], "l8");
    }
    {
        auto& values = node->get_multi_property("A4");
        LIBBOARDGAME_CHECK_EQUAL(values.size(), 1u);
        LIBBOARDGAME_CHECK_EQUAL(values[0], "i13");
    }
    {
        auto& value = node->get_property("PL");
        LIBBOARDGAME_CHECK_EQUAL(value, "2");

        node = &node->get_first_child();
        LIBBOARDGAME_CHECK(! node->has_children());
    }
    {
        auto& value = node->get_property("2");
        LIBBOARDGAME_CHECK_EQUAL(value, "p14");
    }
}

/** Check that keep_only_subtree() works in Nexos.
    Regression test for a bug in Pentobi 12.0 */
LIBBOARDGAME_TEST_CASE(pentobi_base_tree_keep_only_subtree_nexos)
{

    istringstream in("(;GM[Nexos Two-Player]"
                     ";1[g16,g18,f19,e20]"
                     ";2[r17,s18,t19,u20]"
                     ";3[t5,s6,s8,r9]"
                     ";4[e6,e8,f9,h9]"
                     ";1[m14,h15,j15,l15])");
    TreeReader reader;
    reader.read(in);
    unique_ptr<SgfNode> root = reader.get_tree_transfer_ownership();
    PentobiTree tree(root);
    auto node = &tree.get_root();
    node = &node->get_first_child();
    node = &node->get_first_child();
    node = &node->get_first_child();
    node = &node->get_first_child();
    LIBBOARDGAME_CHECK(! tree.is_modified());
    tree.keep_only_subtree(*node);
    LIBBOARDGAME_CHECK(tree.is_modified());

    node = &tree.get_root();
    LIBBOARDGAME_CHECK(node->has_single_child());
    auto values = node->get_multi_property("A1");
    LIBBOARDGAME_CHECK_EQUAL(values.size(), 1u);
    LIBBOARDGAME_CHECK_EQUAL(values[0], "g16,g18,f19,e20");
    values = node->get_multi_property("A2");
    LIBBOARDGAME_CHECK_EQUAL(values.size(), 1u);
    LIBBOARDGAME_CHECK_EQUAL(values[0], "r17,s18,t19,u20");
    values = node->get_multi_property("A3");
    LIBBOARDGAME_CHECK_EQUAL(values.size(), 1u);
    LIBBOARDGAME_CHECK_EQUAL(values[0], "t5,s6,s8,r9");
    values = node->get_multi_property("A4");
    LIBBOARDGAME_CHECK_EQUAL(values.size(), 1u);
    LIBBOARDGAME_CHECK_EQUAL(values[0], "e6,e8,f9,h9");
    auto value = node->get_property("PL");
    LIBBOARDGAME_CHECK_EQUAL(value, "1");

    node = &node->get_first_child();
    LIBBOARDGAME_CHECK(! node->has_children());
    value = node->get_property("1");
    LIBBOARDGAME_CHECK_EQUAL(value, "m14,h15,j15,l15");
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
