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
    log(file, ":", line, ": Assertion '", expression, "' failed");
    get_log() << flush;
    for_each(get_all_handlers().begin(), get_all_handlers().end(),
             mem_fun(&AssertionHandler::run));
    abort();
}

#endif

//-----------------------------------------------------------------------------

} // namespace libboardgame_util
