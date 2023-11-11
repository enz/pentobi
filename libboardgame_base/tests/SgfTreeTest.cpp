//-----------------------------------------------------------------------------
/** @file libboardgame_base/tests/SgfTreeTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_base/SgfTree.h"

#include "libboardgame_test/Test.h"

using namespace libboardgame_base;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(sgf_tree_delete_all_variations)
{
    // root - node1 - node2 - node3
    //              \ node4
    SgfTree tree;
    auto& root = tree.get_root();
    auto& node1 = tree.create_new_child(root);
    auto& node2 = tree.create_new_child(node1);
    auto& node3 = tree.create_new_child(node2);
    auto& node4 = tree.create_new_child(node1);
    LIBBOARDGAME_CHECK_EQUAL(root.get_nu_children(), 1u)
    LIBBOARDGAME_CHECK_EQUAL(node1.get_nu_children(), 2u)
    LIBBOARDGAME_CHECK_EQUAL(node2.get_nu_children(), 1u)
    LIBBOARDGAME_CHECK_EQUAL(node3.get_nu_children(), 0u)
    LIBBOARDGAME_CHECK_EQUAL(node4.get_nu_children(), 0u)
    tree.clear_modified();
    LIBBOARDGAME_CHECK(! tree.is_modified())
    tree.delete_all_variations();
    LIBBOARDGAME_CHECK(tree.is_modified())
    LIBBOARDGAME_CHECK_EQUAL(root.get_nu_children(), 1u)
    LIBBOARDGAME_CHECK_EQUAL(node1.get_nu_children(), 1u)
    LIBBOARDGAME_CHECK_EQUAL(node2.get_nu_children(), 1u)
    LIBBOARDGAME_CHECK_EQUAL(node3.get_nu_children(), 0u)
}

//-----------------------------------------------------------------------------
