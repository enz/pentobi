//-----------------------------------------------------------------------------
/** @file Assert.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Assert.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <boost/format.hpp>
#include "Log.h"

namespace libboardgame_util {

using namespace std;
using boost::format;

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

AssertionHandler::~AssertionHandler() throw()
{
    get_all_handlers().remove(this);
}

//----------------------------------------------------------------------------

#if LIBBOARDGAME_DEBUG

void handle_assertion(const char* expression, const char* file, int line)
{
    static bool is_during_handle_assertion = false;
    static vector<string> additional_assertions;
    string message = str(format("%1%:%2%: Assertion '%3%' failed")
                         % file % line % expression);
    if (is_during_handle_assertion)
    {
        if (find(additional_assertions.begin(), additional_assertions.end(),
                 message) == additional_assertions.end())
            additional_assertions.push_back(message);
        return;
    }
    is_during_handle_assertion = true;
    log(message);
    for_each(get_all_handlers().begin(), get_all_handlers().end(),
             mem_fun(&AssertionHandler::run));
    if (! additional_assertions.empty())
    {
        log("Assertions triggered during execution of assertion handlers:");
        for (const string& s : additional_assertions)
            log(s);
    }
    abort();
}

#endif

//-----------------------------------------------------------------------------

} // namespace libboardgame_util
