//-----------------------------------------------------------------------------
/** @file libboardgame_util/Assert.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_ASSERT_H
#define LIBBOARDGAME_UTIL_ASSERT_H

namespace libboardgame_util {

//-----------------------------------------------------------------------------

class AssertionHandler
{
public:
    /** Construct and register assertion handler. */
    AssertionHandler();

    /** Destruct and unregister assertion handler. */
    virtual ~AssertionHandler() throw();

    virtual void run() = 0;
};

void handle_assertion(const char* expression, const char* file, int line);

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

//-----------------------------------------------------------------------------

/** @def LIBBOARDGAME_DEBUG
    Determines if the assertions with LIBBOARDGAME_ASSERT are enabled.
    LIBBOARDGAME_DEBUG needs to be defined and set to a value that is not zero
    to enable assertions. If LIBBOARDGAME_DEBUG is not explicitely defined it
    will default to 1 if either DEBUG or _DEBUG are defined (independent of
    their value) and 0 otherwise.
    @warning The definition of LIBBOARDGAME_DEBUG must be consistent in all
    compilation units because the debug versions of some classes in
    LibBoardGame have additional members and the program will crash if
    compilation units are linked that do not agree on the size of objects. */
#ifndef LIBBOARDGAME_DEBUG
#if defined(DEBUG) || defined(_DEBUG)
#define LIBBOARDGAME_DEBUG 1
#else
#define LIBBOARDGAME_DEBUG 0
#endif
#endif

/** @def LIBBOARDGAME_ASSERT
    Enhanced assert macro.
    This macro is similar to the assert macro in the standard library, but it
    allows to register assertion handlers that are executed before the program
    is aborted. Assertions are only enabled if LIBBOARDGAME_DEBUG is defined
    and not zero. */
#if LIBBOARDGAME_DEBUG
#define LIBBOARDGAME_ASSERT(expr)                                       \
    ((expr) ? (static_cast<void>(0))                                    \
     : libboardgame_util::handle_assertion(#expr, __FILE__, __LINE__))
#else
#define LIBBOARDGAME_ASSERT(expr) (static_cast<void>(0))
#endif

//-----------------------------------------------------------------------------

#endif // LIBBOARDGAME_UTIL_ASSERT_H
