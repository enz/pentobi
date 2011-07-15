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

map<string, TestFunction> all_tests;

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
    LIBBOARDGAME_ASSERT(all_tests.find(name) == all_tests.end());
    all_tests.insert(make_pair(name, function));
}

bool run_all_tests()
{
    unsigned int nu_fail = 0;
    log() << "Running " << all_tests.size() << " tests ...\n";
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

//-----------------------------------------------------------------------------

} // namespace libboardgame_test
