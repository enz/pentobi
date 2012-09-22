//-----------------------------------------------------------------------------
/** @file libboardgame_test/Test.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Test.h"

#include <map>
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/Log.h"

using libboardgame_util::log;

namespace libboardgame_test {

using namespace std;

//-----------------------------------------------------------------------------

namespace {

map<string, TestFunction>& get_all_tests()
{
    static map<string, TestFunction> all_tests;
    return all_tests;
}

} // namespace

//-----------------------------------------------------------------------------

TestFail::TestFail(const char* file, int line, const string& s)
    : Exception(format("%1%:%2%: %3%") % file % line % s)
{
}

TestFail::TestFail(const char* file, int line, const format& f)
    : Exception(format("%1%:%2%: %3%") % file % line % str(f))
{
}

//-----------------------------------------------------------------------------

void add_test(const string& name, TestFunction function)
{
    map<string, TestFunction>& all_tests = get_all_tests();
    LIBBOARDGAME_ASSERT(all_tests.find(name) == all_tests.end());
    all_tests.insert(make_pair(name, function));
}

bool run_all_tests()
{
    unsigned nu_fail = 0;
    log() << "Running " << get_all_tests().size() << " tests...\n";
    auto all_tests = get_all_tests();
    for (auto i = all_tests.begin(); i != all_tests.end(); ++i)
    {
        try
        {
            (i->second)();
        }
        catch (const TestFail& e)
        {
            log() << e.what() << '\n';
            ++nu_fail;
        }
    }
    if (nu_fail == 0)
    {
        log() << "OK\n";
        return true;
    }
    else
    {
        log() << nu_fail << " tests failed.\n"
              << "FAIL\n";
        return false;
    }
}

bool run_test(const string& name)
{
    auto all_tests = get_all_tests();
    for (auto i = all_tests.begin(); i != all_tests.end(); ++i)
        if (i->first == name)
        {
            log() << "Running " << name << "...\n";
            try
            {
                (i->second)();
                log() << "OK\n";
                return true;
            }
            catch (const TestFail& e)
            {
                log() << e.what() << '\n'
                      << "FAIL\n";
                return false;
            }
        }
    log() << "Test not found: " << name;
    return false;
}

int test_main(int argc, char* argv[])
{
    if (argc < 2)
    {
        if (libboardgame_test::run_all_tests())
            return 0;
        else
            return 1;
    }
    else
    {
        int result = 0;
        for (int i = 1; i < argc; ++i)
            if (! libboardgame_test::run_test(argv[i]))
                result = 1;
        return result;
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_test
