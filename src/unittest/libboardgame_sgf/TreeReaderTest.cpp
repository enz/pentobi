//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_sgf/TreeReaderTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

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
    auto& root = reader.get_tree();
    LIBBOARDGAME_CHECK(root.has_property("B"));
    LIBBOARDGAME_CHECK(root.has_single_child());
    auto& child = root.get_child();
    LIBBOARDGAME_CHECK(child.has_property("W"));
    LIBBOARDGAME_CHECK(! child.has_children());
}

LIBBOARDGAME_TEST_CASE(sgf_tree_reader_basic_2)
{
    istringstream in("(;C[1](;C[2.1])(;C[2.2]))");
    TreeReader reader;
    reader.read(in);
    auto& root = reader.get_tree();
    LIBBOARDGAME_CHECK_EQUAL(root.get_property("C"), "1");
    LIBBOARDGAME_CHECK_EQUAL(root.get_nu_children(), 2u);
    LIBBOARDGAME_CHECK_EQUAL(root.get_child(0).get_property("C"), "2.1");
    LIBBOARDGAME_CHECK_EQUAL(root.get_child(1).get_property("C"), "2.2");
}

LIBBOARDGAME_TEST_CASE(sgf_tree_reader_multiprop_with_whitespace)
{
    istringstream in("(;A [1]\n[2] [3]\t[4]\r\n[5])");
    TreeReader reader;
    reader.read(in);
    auto& root = reader.get_tree();
    auto values = root.get_multi_property("A");
    LIBBOARDGAME_CHECK_EQUAL(values.size(), 5u);
    LIBBOARDGAME_CHECK_EQUAL(values[0], "1");
    LIBBOARDGAME_CHECK_EQUAL(values[1], "2");
    LIBBOARDGAME_CHECK_EQUAL(values[2], "3");
    LIBBOARDGAME_CHECK_EQUAL(values[3], "4");
    LIBBOARDGAME_CHECK_EQUAL(values[4], "5");
}

/** Test that a property value with a unicode character is preserved after
    reading and writing.
    In previous versions this was broken because of a bug in the replacement
    of non-newline whitespaces (as required by SGF) by the writer. (The bug
    occurred only on some platforms depending on the std::isspace()
    implementation.) */
LIBBOARDGAME_TEST_CASE(sgf_tree_reader_unicode)
{
    SgfNode root;
    const char* id = "C";
    const char* value = "ü";
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
    auto& root = reader.get_tree();
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
        auto& root = reader.get_tree();
        LIBBOARDGAME_CHECK_EQUAL(root.get_property("C"), "1\n2");
    }
    {
        istringstream in("(;C[1\r\n2])");
        TreeReader reader;
        reader.read(in);
        auto& root = reader.get_tree();
        LIBBOARDGAME_CHECK_EQUAL(root.get_property("C"), "1\n2");
    }
    {
        istringstream in("(;C[1\r2])");
        TreeReader reader;
        reader.read(in);
        auto& root = reader.get_tree();
        LIBBOARDGAME_CHECK_EQUAL(root.get_property("C"), "1\n2");
    }
}

LIBBOARDGAME_TEST_CASE(sgf_tree_reader_property_without_value)
{
    istringstream in("(;B)");
    TreeReader reader;
    LIBBOARDGAME_CHECK_THROW(reader.read(in), TreeReader::ReadError);
}

LIBBOARDGAME_TEST_CASE(sgf_tree_reader_text_before_node)
{
    istringstream in("(B;)");
    TreeReader reader;
    LIBBOARDGAME_CHECK_THROW(reader.read(in), TreeReader::ReadError);
}

//-----------------------------------------------------------------------------
