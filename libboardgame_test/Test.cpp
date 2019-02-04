//-----------------------------------------------------------------------------
/** @file libboardgame_test/Test.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Test.h"

#include <map>
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/Log.h"

namespace libboardgame_test {

//-----------------------------------------------------------------------------

namespace {

map<string, TestFunction>& get_all_tests()
{
    static map<string, TestFunction> all_tests;
    return all_tests;
}

string get_fail_msg(const char* file, int line, const string& s)
{
    ostringstream msg;
    msg << file << ":" << line << ": " << s;
    return msg.str();
}

} // namespace

//-----------------------------------------------------------------------------

TestFail::TestFail(const char* file, int line, const string& s)
    : logic_error(get_fail_msg(file, line, s))
{
}

//-----------------------------------------------------------------------------

void add_test(const string& name, const TestFunction& function)
{
    auto& all_tests = get_all_tests();
    LIBBOARDGAME_ASSERT(all_tests.find(name) == all_tests.end());
    all_tests.insert({name, function});
}

bool run_all_tests()
{
    unsigned nu_fail = 0;
    LIBBOARDGAME_LOG("Running ", get_all_tests().size(), " tests...");
    for (auto& i : get_all_tests())
    {
        try
        {
            (i.second)();
        }
        catch (const TestFail& e)
        {
            LIBBOARDGAME_LOG(e.what());
            ++nu_fail;
        }
    }
    if (nu_fail == 0)
    {
        LIBBOARDGAME_LOG("OK");
        return true;
    }
    LIBBOARDGAME_LOG(nu_fail, " tests failed.\nFAIL");
    return false;
}

bool run_test(const string& name)
{
    for (auto& i : get_all_tests())
        if (i.first == name)
        {
            LIBBOARDGAME_LOG("Running ", name, "...");
            try
            {
                (i.second)();
                LIBBOARDGAME_LOG("OK");
                return true;
            }
            catch (const TestFail& e)
            {
                LIBBOARDGAME_LOG(e.what(), "\nFAIL");
                return false;
            }
        }
    LIBBOARDGAME_LOG("Test not found: ", name);
    return false;
}

int test_main(int argc, char* argv[])
{
    libboardgame_util::LogInitializer log_initializer;
    if (argc < 2)
        return run_all_tests() ? 0 : 1;
    int result = 0;
    for (int i = 1; i < argc; ++i)
        if (! run_test(argv[i]))
            result = 1;
    return result;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_test
