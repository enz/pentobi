//-----------------------------------------------------------------------------
/** @file libboardgame_util/Assert.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Assert.h"

#include <list>

#ifdef LIBBOARDGAME_DEBUG
#include <algorithm>
#include <functional>
#include <sstream>
#include <string>
#include <vector>
#include "Log.h"
#endif

#ifdef LIBBOARDGAME_DISABLE_LOG
#include "Unused.h"
#endif

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

namespace {

list<AssertionHandler*>& get_all_handlers()
{
    static list<AssertionHandler*> all_handlers;
    return all_handlers;
}

} // namespace

//----------------------------------------------------------------------------

AssertionHandler::AssertionHandler()
{
    get_all_handlers().push_back(this);
}

AssertionHandler::~AssertionHandler()
{
    get_all_handlers().remove(this);
}

//----------------------------------------------------------------------------

#ifdef LIBBOARDGAME_DEBUG

void handle_assertion(const char* expression, const char* file, int line)
{
    static bool is_during_handle_assertion = false;
#ifdef LIBBOARDGAME_DISABLE_LOG
    LIBBOARDGAME_UNUSED(expression);
    LIBBOARDGAME_UNUSED(file);
    LIBBOARDGAME_UNUSED(line);
#else
    LIBBOARDGAME_LOG(file, ":", line, ": Assertion '", expression, "' failed");
#endif
    flush_log();
    if (! is_during_handle_assertion)
    {
        is_during_handle_assertion = true;
        for_each(get_all_handlers().begin(), get_all_handlers().end(),
                 mem_fun(&AssertionHandler::run));
    }
    abort();
}

#endif

//-----------------------------------------------------------------------------

} // namespace libboardgame_util
