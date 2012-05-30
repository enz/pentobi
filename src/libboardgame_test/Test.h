//-----------------------------------------------------------------------------
/** @file libboardgame_test/Test.h
    Provides functionality similar to Boost.Test.

    The main reason that Boost.Test is not used is that it has the flaw that
    the way how the main function is created depends on whether static or
    dynamic versions of the Boost libraries are used, which we don't know if
    the libraries are auto-detected. While it was possible in earlier versions
    of Boost to disable the main function from the library in any of the two
    cases and provide our own, this no longer seems to be possible with
    Boost 1.48.
    A second reason is that it might be desirable to drop the dependence
    on Boost completely in the future. */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_TEST_TEST_H
#define LIBBOARDGAME_TEST_TEST_H

#include <cmath>
#include <string>
#include "libboardgame_util/Exception.h"

namespace libboardgame_test {

using namespace std;
using boost::format;
using libboardgame_util::Exception;

//-----------------------------------------------------------------------------

typedef void (*TestFunction)();

//-----------------------------------------------------------------------------

class TestFail
    : public Exception
{
public:
    TestFail(const char* file, int line, const string& s);

    TestFail(const char* file, int line, const format& f);
};

//-----------------------------------------------------------------------------

void add_test(const string& name, TestFunction function);

bool run_all_tests();

bool run_test(const string& name);

/** Main function that runs all tests (if no arguments) or only the tests
    given as arguments. */
int test_main(int argc, char* argv[]);

//-----------------------------------------------------------------------------

/** Helper class that automatically adds a test when and instance isdeclared. */
struct TestRegistrar
{
    TestRegistrar(const string& name, TestFunction function)
    {
        add_test(name, function);
    }
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_test

//-----------------------------------------------------------------------------

#define LIBBOARDGAME_TEST_CASE(name)                                    \
    void name();                                                        \
    libboardgame_test::TestRegistrar name##_registrar(#name, name);     \
    void name()


#define LIBBOARDGAME_CHECK(expr)                                        \
    if (! (expr))                                                       \
        throw libboardgame_test::TestFail(__FILE__, __LINE__, "check failed")

#define LIBBOARDGAME_CHECK_EQUAL(expr1, expr2)                          \
    {                                                                   \
        using libboardgame_test::TestFail;                              \
        auto result1 = (expr1);                                         \
        auto result2 = (expr2);                                         \
        if (result1 != result2)                                         \
            throw TestFail(__FILE__, __LINE__,                          \
                           boost::format("'%1%' != '%2%'")              \
                           % result1 % result2);                        \
    }

#define LIBBOARDGAME_CHECK_THROW(expr, exception)                       \
    {                                                                   \
        using libboardgame_test::TestFail;                              \
        bool was_thrown = false;                                        \
        try                                                             \
        {                                                               \
            expr;                                                       \
        }                                                               \
        catch (const exception&)                                        \
        {                                                               \
            was_thrown = true;                                          \
        }                                                               \
        if (! was_thrown)                                               \
            throw TestFail(__FILE__, __LINE__,                          \
                           boost::format("Exception '%1%' was not thrown") \
                           % #exception);                               \
    }

#define LIBBOARDGAME_CHECK_NO_THROW(expr)                               \
    {                                                                   \
        using libboardgame_test::TestFail;                              \
        try                                                             \
        {                                                               \
            expr;                                                       \
        }                                                               \
        catch (...)                                                     \
        {                                                               \
            throw TestFail(__FILE__, __LINE__,                          \
                           "Unexcpected exception was thrown");         \
        }                                                               \
    }

/** Compare floating points using a tolerance in percent. */
#define LIBBOARDGAME_CHECK_CLOSE(expr1, expr2, tolerance)               \
    {                                                                   \
        using libboardgame_test::TestFail;                              \
        auto result1 = (expr1);                                         \
        auto result2 = (expr2);                                         \
        if (fabs(result1 - result2) > 0.01 * tolerance * result1)       \
            throw TestFail(__FILE__, __LINE__,                          \
                           boost::format("Difference between %1% and"   \
                                         " %2% exceeds %3% percent")    \
                           % result1 % result2 % (0.01 * tolerance));   \
    }

/** Compare floating points using an epsilon. */
#define LIBBOARDGAME_CHECK_CLOSE_EPS(expr1, expr2, epsilon)             \
    {                                                                   \
        using libboardgame_test::TestFail;                              \
        auto result1 = (expr1);                                         \
        auto result2 = (expr2);                                         \
        if (fabs(result1 - result2) > epsilon)                          \
            throw TestFail(__FILE__, __LINE__,                          \
                           boost::format("Difference between %1% and"   \
                                         " %2% exceeds %3%")            \
                           % result1 % result2 % epsilon);              \
    }

//-----------------------------------------------------------------------------

#endif // LIBBOARDGAME_TEST_TEST_H
