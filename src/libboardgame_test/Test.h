//-----------------------------------------------------------------------------
/** @file libboardgame_test/Test.h
    Provides functionality similar to Boost.Test.
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_TEST_TEST_H
#define LIBBOARDGAME_TEST_TEST_H

#include <cmath>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>

namespace libboardgame_test {

using namespace std;

//-----------------------------------------------------------------------------

using TestFunction = function<void()>;

//-----------------------------------------------------------------------------

class TestFail
    : public logic_error
{
public:
    TestFail(const char* file, int line, const string& s);
};

//-----------------------------------------------------------------------------

void add_test(const string& name, const TestFunction& function);

bool run_all_tests();

bool run_test(const string& name);

/** Main function that runs all tests (if no arguments) or only the tests
    given as arguments. */
int test_main(int argc, char* argv[]);

//-----------------------------------------------------------------------------

/** Helper class that automatically adds a test when an instance is
    declared. */
struct TestRegistrar
{
    TestRegistrar(const string& name, const TestFunction& function)
    {
        add_test(name, function);
    }
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_test

//-----------------------------------------------------------------------------

#define LIBBOARDGAME_TEST_CASE(name)                                       \
    static void name();                                                    \
    static libboardgame_test::TestRegistrar name##_registrar(#name, name); \
    void name()


#define LIBBOARDGAME_CHECK(expr)                                        \
    if (! (expr))                                                       \
        throw libboardgame_test::TestFail(__FILE__, __LINE__, "check failed")

#define LIBBOARDGAME_CHECK_EQUAL(expr1, expr2)                          \
    {                                                                   \
        using libboardgame_test::TestFail;                              \
        const auto& result1 = (expr1);                                         \
        const auto& result2 = (expr2);                                         \
        if (result1 != result2)                                         \
        {                                                               \
            ostringstream msg;                                          \
            msg << "'" << result1 << "' != '" << result2 << "'";   \
            throw TestFail(__FILE__, __LINE__, msg.str());              \
        }                                                               \
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
        {                                                               \
            ostringstream msg;                                          \
            msg << "Exception '" << #exception << "' was not thrown";   \
            throw TestFail(__FILE__, __LINE__, msg.str());              \
        }                                                               \
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
        if (fabs(result1 - result2) > 0.01 * (tolerance) * result1)     \
        {                                                               \
            ostringstream msg;                                          \
            msg << "Difference between " << result1 << " and "          \
                << result2 << " exceeds " << (0.01 * (tolerance))       \
                << " percent";                                          \
            throw TestFail(__FILE__, __LINE__, msg.str());              \
        }                                                               \
    }

/** Compare floating points using an epsilon. */
#define LIBBOARDGAME_CHECK_CLOSE_EPS(expr1, expr2, epsilon)             \
    {                                                                   \
        using libboardgame_test::TestFail;                              \
        auto result1 = (expr1);                                         \
        auto result2 = (expr2);                                         \
        if (fabs(result1 - result2) > (epsilon))                        \
        {                                                               \
            ostringstream msg;                                          \
            msg << "Difference between " << result1 << " and "          \
                << result2 << " exceeds " << (epsilon);                 \
            throw TestFail(__FILE__, __LINE__, msg.str());              \
        }                                                               \
    }

//-----------------------------------------------------------------------------

#endif // LIBBOARDGAME_TEST_TEST_H
