//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_sgf/TreeIteratorTest.cpp
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_sgf/Tree.h"
#include "libboardgame_sgf/TreeIterator.h"
#include "libboardgame_test/Test.h"

using namespace libboardgame_sgf;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgamesgf_tree_iterator_1)
{
    Tree tree;
    auto& root = tree.get_root();
    TreeIterator i(root);
    LIBBOARDGAME_CHECK(i);
    LIBBOARDGAME_CHECK_EQUAL(&(*i), &root);
    ++i;
    LIBBOARDGAME_CHECK(! i);
}

LIBBOARDGAME_TEST_CASE(boardgamesgf_tree_iterator_2)
{
    Tree tree;
    auto& root = tree.get_root();
    auto& child1 = tree.create_new_child(root);
    auto& child2 = tree.create_new_child(root);
    auto& child3 = tree.create_new_child(child1);
    TreeIterator i(root);
    LIBBOARDGAME_CHECK(i);
    LIBBOARDGAME_CHECK_EQUAL(&(*i), &root);
    ++i;
    LIBBOARDGAME_CHECK(i);
    LIBBOARDGAME_CHECK_EQUAL(&(*i), &child1);
    ++i;
    LIBBOARDGAME_CHECK(i);
    LIBBOARDGAME_CHECK_EQUAL(&(*i), &child3);
    ++i;
    LIBBOARDGAME_CHECK(i);
    LIBBOARDGAME_CHECK_EQUAL(&(*i), &child2);
    ++i;
    LIBBOARDGAME_CHECK(! i);
}

//-----------------------------------------------------------------------------
