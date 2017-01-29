//-----------------------------------------------------------------------------
/** @file libboardgame_util/Assert.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Assert.h"

#include <list>
#include "Unused.h"

#if LIBBOARDGAME_DEBUG
#include <algorithm>
#include <functional>
#include <sstream>
#include <string>
#include <vector>
#include "Log.h"
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

#if LIBBOARDGAME_DEBUG

void handle_assertion(const char* expression, const char* file, int line)
{
    static bool is_during_handle_assertion = false;
#if LIBBOARDGAME_DISABLE_LOG
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
