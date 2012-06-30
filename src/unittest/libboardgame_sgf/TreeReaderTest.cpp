//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_sgf/TreeReaderTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_sgf/TreeReader.h"

#include <sstream>
#include "libboardgame_sgf/TreeWriter.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_sgf;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(sgf_tree_reader_basic)
{
    istringstream in("(;B[aa];W[bb])");
    TreeReader reader;
    reader.read(in);
    const Node& root = reader.get_tree();
    LIBBOARDGAME_CHECK(root.has_property("B"));
    LIBBOARDGAME_CHECK(root.has_single_child());
    const Node& child = root.get_child();
    LIBBOARDGAME_CHECK(child.has_property("W"));
    LIBBOARDGAME_CHECK(! child.has_children());
}

LIBBOARDGAME_TEST_CASE(sgf_tree_reader_basic_2)
{
    istringstream in("(;C[1](;C[2.1])(;C[2.2]))");
    TreeReader reader;
    reader.read(in);
    const Node& root = reader.get_tree();
    LIBBOARDGAME_CHECK_EQUAL(root.get_property("C"), "1");
    LIBBOARDGAME_CHECK_EQUAL(root.get_nu_children(), 2u);
    LIBBOARDGAME_CHECK_EQUAL(root.get_child(0).get_property("C"), "2.1");
    LIBBOARDGAME_CHECK_EQUAL(root.get_child(1).get_property("C"), "2.2");
}

/** Test that a property value with a unicode character is preserved after
    reading and writing.
    In previous versions this was broken because of a bug in the replacement
    of non-newline whitespaces (as required by SGF) by the writer. (The bug
    occurred only on some platforms depending on the std::isspace()
    implementation.) */
LIBBOARDGAME_TEST_CASE(sgf_tree_reader_unicode)
{
    Node root;
    const char* id = "C";
    const char* value = "\xc3\xbc"; // German u-umlaut as UTF-8
    root.set_property(id, value);
    ostringstream out;
    TreeWriter writer(out, root);
    writer.write();
    istringstream in(out.str());
    TreeReader reader;
    reader.read(in);
    LIBBOARDGAME_CHECK_EQUAL(reader.get_tree().get_property(id), value);
}

LIBBOARDGAME_TEST_CASE(sgf_tree_reader_property_after_newline)
{
    istringstream in("(;FF[4]\n"
                     "CA[UTF-8])");
    TreeReader reader;
    reader.read(in);
    const Node& root = reader.get_tree();
    LIBBOARDGAME_CHECK(root.has_property("FF"));
    LIBBOARDGAME_CHECK(root.has_property("CA"));
}

/** Test cross-platform handling of property values containing newlines.
    The reader should convert all platform-dependent newline sequences (LF,
    CR+LF, CR) into LF, such that property values containing newlines are
    independent on the platform that was used to write the file. */
LIBBOARDGAME_TEST_CASE(sgf_tree_reader_newline)
{
    {
        istringstream in("(;C[1\n2])");
        TreeReader reader;
        reader.read(in);
        const Node& root = reader.get_tree();
        LIBBOARDGAME_CHECK_EQUAL(root.get_property("C"), "1\n2");
    }
    {
        istringstream in("(;C[1\r\n2])");
        TreeReader reader;
        reader.read(in);
        const Node& root = reader.get_tree();
        LIBBOARDGAME_CHECK_EQUAL(root.get_property("C"), "1\n2");
    }
    {
        istringstream in("(;C[1\r2])");
        TreeReader reader;
        reader.read(in);
        const Node& root = reader.get_tree();
        LIBBOARDGAME_CHECK_EQUAL(root.get_property("C"), "1\n2");
    }
}

//-----------------------------------------------------------------------------
