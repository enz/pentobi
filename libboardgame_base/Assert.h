//-----------------------------------------------------------------------------
/** @file libboardgame_base/Assert.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_ASSERT_H
#define LIBBOARDGAME_BASE_ASSERT_H

namespace libboardgame_base {

//-----------------------------------------------------------------------------

class AssertionHandler
{
public:
    /** Construct and register assertion handler. */
    AssertionHandler();

    /** Destruct and unregister assertion handler. */
    virtual ~AssertionHandler();

    AssertionHandler(const AssertionHandler&) = delete;
    AssertionHandler& operator=(const AssertionHandler&) = delete;

    virtual void run() = 0;
};

#ifdef LIBBOARDGAME_DEBUG

/** Function used by the LIBBOARDGAME_ASSERT macro to run all assertion
    handlers. */
[[noreturn]] void handle_assertion(const char* expression, const char* file,
                                   int line);

#endif

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

//-----------------------------------------------------------------------------

/** @def LIBBOARDGAME_ASSERT
    Enhanced assert macro.
    This macro is similar to the assert macro in the standard library, but it
    allows the user to register assertion handlers that are executed before the
    program is aborted. Assertions are only enabled if the macro
    LIBBOARDGAME_DEBUG is true. */
#ifdef LIBBOARDGAME_DEBUG
#define LIBBOARDGAME_ASSERT(expr)                                       \
    ((expr) ? (static_cast<void>(0))                                    \
     : libboardgame_base::handle_assertion(#expr, __FILE__, __LINE__))
#else
#define LIBBOARDGAME_ASSERT(expr) (static_cast<void>(0))
#endif

//-----------------------------------------------------------------------------

#endif // LIBBOARDGAME_BASE_ASSERT_H
