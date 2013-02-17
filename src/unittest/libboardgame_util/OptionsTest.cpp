//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_util/OptionsTest.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_util/Options.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_util;

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(libboardgame_util_options_basic)
{
    vector<string> specs;
    specs.push_back("first|a:");
    specs.push_back("second|b:");
    specs.push_back("third|c");
    specs.push_back("fourth");
    specs.push_back("fifth");
    const char* argv[] =
        { nullptr, "--second", "secondval", "--first", "firstval",
          "--fourth", "-c", "arg1", "arg2" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    Options opt(argc, argv, specs);
    LIBBOARDGAME_CHECK(opt.contains("first"));
    LIBBOARDGAME_CHECK_EQUAL(opt.get("first"), "firstval");
    LIBBOARDGAME_CHECK(opt.contains("second"));
    LIBBOARDGAME_CHECK_EQUAL(opt.get("second"), "secondval");
    LIBBOARDGAME_CHECK(opt.contains("third"));
    LIBBOARDGAME_CHECK(opt.contains("fourth"));
    LIBBOARDGAME_CHECK(! opt.contains("fifth"));
    auto& args = opt.get_args();
    LIBBOARDGAME_CHECK_EQUAL(args.size(), 2u);
    LIBBOARDGAME_CHECK_EQUAL(args[0], "arg1");
    LIBBOARDGAME_CHECK_EQUAL(args[1], "arg2");
}

LIBBOARDGAME_TEST_CASE(libboardgame_util_options_end_options)
{
    vector<string> specs;
    specs.push_back("first:");
    const char* argv[] =
        { nullptr, "--first", "firstval", "--", "--arg1" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    Options opt(argc, argv, specs);
    LIBBOARDGAME_CHECK_EQUAL(opt.get("first"), "firstval");
    auto& args = opt.get_args();
    LIBBOARDGAME_CHECK_EQUAL(args.size(), 1u);
    LIBBOARDGAME_CHECK_EQUAL(args[0], "--arg1");
}

LIBBOARDGAME_TEST_CASE(libboardgame_util_options_nospace)
{
    vector<string> specs;
    specs.push_back("first|a:");
    specs.push_back("second|b:");
    const char* argv[] = { nullptr, "-abc" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    Options opt(argc, argv, specs);
    LIBBOARDGAME_CHECK_EQUAL(opt.get("first"), "bc");
}

LIBBOARDGAME_TEST_CASE(libboardgame_util_options_multi_short_with_val)
{
    vector<string> specs;
    specs.push_back("first|a");
    specs.push_back("second|b:");
    const char* argv[] = { nullptr, "-ab", "c" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    Options opt(argc, argv, specs);
    LIBBOARDGAME_CHECK(opt.contains("first"));
    LIBBOARDGAME_CHECK_EQUAL(opt.get("second"), "c");
}

LIBBOARDGAME_TEST_CASE(libboardgame_util_options_type)
{
    vector<string> specs;
    specs.push_back("first:");
    specs.push_back("second:");
    const char* argv[] = { nullptr, "--first", "10", "--second", "foo" };
    int argc = sizeof(argv) / sizeof(argv[0]);
    Options opt(argc, argv, specs);
    LIBBOARDGAME_CHECK_EQUAL(opt.get<int>("first"), 10);
    LIBBOARDGAME_CHECK_THROW(opt.get<int>("second"), Exception);
}

//-----------------------------------------------------------------------------
