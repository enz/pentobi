//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/TreeTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_sgf/Tree.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_sgf;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(boardgame_sgf_tree_gas_comment_property_int)
{
    Tree tree;
    auto& n = tree.get_root();
    tree.set_comment(n, "a=5");
    LIBBOARDGAME_CHECK_EQUAL(tree.get_comment_property<int>(n, "a", 0), 5);
}

LIBBOARDGAME_TEST_CASE(boardgame_sgf_tree_has_comment_property)
{
    Tree tree;
    auto& n = tree.get_root();
    tree.set_comment(n, "key=value");
    LIBBOARDGAME_CHECK(tree.has_comment_property(n, "key"));
}

LIBBOARDGAME_TEST_CASE(boardgame_sgf_tree_set_comment_property_new)
{
    Tree tree;
    auto& n = tree.get_root();
    tree.set_comment_property(n, "key", "value");
    LIBBOARDGAME_CHECK_EQUAL(tree.get_comment(n), "key=value\n");
}

LIBBOARDGAME_TEST_CASE(boardgame_sgf_tree_set_comment_property_new_float)
{
    Tree tree;
    auto& n = tree.get_root();
    tree.set_comment_property(n, "key", 1.234);
    LIBBOARDGAME_CHECK_EQUAL(tree.get_comment(n), "key=1.234\n");
}

LIBBOARDGAME_TEST_CASE(boardgame_sgf_tree_set_comment_property_replace_end)
{
    Tree tree;
    auto& n = tree.get_root();
    tree.set_comment(n,
                     "key1=value1\n"
                     "key2=value2\n");
    tree.set_comment_property(n, "key2", "new_value2");
    LIBBOARDGAME_CHECK_EQUAL(tree.get_comment(n),
                             "key1=value1\n"
                             "key2=new_value2\n");
}

LIBBOARDGAME_TEST_CASE(boardgame_sgf_tree_set_comment_replace_middle)
{
    Tree tree;
    auto& n = tree.get_root();
    tree.set_comment(n,
                     "key1=value1\n"
                     "key2=value2\n"
                     "key3=value3\n");
    tree.set_comment_property(n, "key2", "new_value2");
    LIBBOARDGAME_CHECK_EQUAL(tree.get_comment(n),
                             "key1=value1\n"
                             "key2=new_value2\n"
                             "key3=value3\n");
}

//-----------------------------------------------------------------------------
