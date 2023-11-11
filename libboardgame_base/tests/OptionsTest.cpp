//-----------------------------------------------------------------------------
/** @file libboardgame_base/tests/OptionsTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_base/Options.h"

#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_base;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(libboardgame_base_options_basic)
{
    vector<string> specs =
        { "first|a:", "second|b:", "third|c", "fourth", "fifth" };
    const char* argv[] =
        { nullptr, "--second", "secondval", "--first", "firstval",
          "--fourth", "-c", "arg1", "arg2" };
    auto argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));
    Options opt(argc, argv, specs);
    LIBBOARDGAME_CHECK(opt.contains("first"))
    LIBBOARDGAME_CHECK_EQUAL(opt.get("first"), "firstval")
    LIBBOARDGAME_CHECK(opt.contains("second"))
    LIBBOARDGAME_CHECK_EQUAL(opt.get("second"), "secondval")
    LIBBOARDGAME_CHECK(opt.contains("third"))
    LIBBOARDGAME_CHECK(opt.contains("fourth"))
    LIBBOARDGAME_CHECK(! opt.contains("fifth"))
    auto& args = opt.get_args();
    LIBBOARDGAME_CHECK_EQUAL(args.size(), 2u)
    LIBBOARDGAME_CHECK_EQUAL(args[0], "arg1")
    LIBBOARDGAME_CHECK_EQUAL(args[1], "arg2")
}

LIBBOARDGAME_TEST_CASE(libboardgame_base_options_end_options)
{
    vector<string> specs = { "first:" };
    const char* argv[] =
        { nullptr, "--first", "firstval", "--", "--arg1" };
    auto argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));
    Options opt(argc, argv, specs);
    LIBBOARDGAME_CHECK_EQUAL(opt.get("first"), "firstval")
    auto& args = opt.get_args();
    LIBBOARDGAME_CHECK_EQUAL(args.size(), 1u)
    LIBBOARDGAME_CHECK_EQUAL(args[0], "--arg1")
}

LIBBOARDGAME_TEST_CASE(libboardgame_base_options_missing_val)
{
    vector<string> specs = { "first:" };
    const char* argv[] = { nullptr, "--first" };
    auto argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));
    LIBBOARDGAME_CHECK_THROW(Options opt(argc, argv, specs), runtime_error)
}

LIBBOARDGAME_TEST_CASE(libboardgame_base_options_nospace)
{
    vector<string> specs = { "first|a:", "second|b:" };
    const char* argv[] = { nullptr, "-abc" };
    auto argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));
    Options opt(argc, argv, specs);
    LIBBOARDGAME_CHECK_EQUAL(opt.get("first"), "bc")
}

LIBBOARDGAME_TEST_CASE(libboardgame_base_options_multi_short_with_val)
{
    vector<string> specs = { "first|a", "second|b:" };
    const char* argv[] = { nullptr, "-ab", "c" };
    auto argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));
    Options opt(argc, argv, specs);
    LIBBOARDGAME_CHECK(opt.contains("first"))
    LIBBOARDGAME_CHECK_EQUAL(opt.get("second"), "c")
}

LIBBOARDGAME_TEST_CASE(libboardgame_base_options_type)
{
    vector<string> specs = { "first:", "second:" };
    const char* argv[] = { nullptr, "--first", "10", "--second", "foo" };
    auto argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));
    Options opt(argc, argv, specs);
    LIBBOARDGAME_CHECK_EQUAL(opt.get<int>("first"), 10)
    LIBBOARDGAME_CHECK_THROW(opt.get<int>("second"), runtime_error)
}

//-----------------------------------------------------------------------------
