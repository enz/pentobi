//-----------------------------------------------------------------------------
/** @file libboardgame_base/Assert.cpp
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

namespace libboardgame_base {

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

void handle_assertion(
        [[maybe_unused]] const char* expression,
        [[maybe_unused]] const char* file, [[maybe_unused]] int line)
{
    static bool is_during_handle_assertion = false;
    LIBBOARDGAME_LOG(file, ":", line, ": Assertion '", expression, "' failed");
    flush_log();
    if (! is_during_handle_assertion)
    {
        is_during_handle_assertion = true;
        for_each(get_all_handlers().begin(), get_all_handlers().end(),
                 mem_fn(&AssertionHandler::run));
    }
    abort();
}

#endif

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
