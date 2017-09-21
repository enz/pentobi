//-----------------------------------------------------------------------------
/** @file unittest/libboardgame_gtp/EngineTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "libboardgame_gtp/Engine.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_gtp;

//-----------------------------------------------------------------------------

namespace {

//-----------------------------------------------------------------------------

/** GTP engine returning invalid responses for testing class Engine.
    For testing that the base class Engine sanitizes responses of
    subclasses that contain empty lines (see @ref Engine::exec_main_loop). */
class InvalidResponseEngine
    : public Engine
{
public:
    InvalidResponseEngine();

    void invalid_response(Arguments, Response&);

    void invalid_response_2(Arguments, Response&);
};

InvalidResponseEngine::InvalidResponseEngine()
{
    add("invalid_response", &InvalidResponseEngine::invalid_response);
    add("invalid_response_2", &InvalidResponseEngine::invalid_response_2);
}

void InvalidResponseEngine::invalid_response(Arguments, Response& r)
{
    r << "This response is invalid\n"
      << "\n"
      << "because it contains an empty line";
}

void InvalidResponseEngine::invalid_response_2(Arguments, Response& r)
{
    r << "This response is invalid\n"
      << "\n"
      << "\n"
      << "because it contains two empty lines";
}

//-----------------------------------------------------------------------------

} // namespace

//-----------------------------------------------------------------------------

LIBBOARDGAME_TEST_CASE(gtp_engine_command)
{
    istringstream in("version\n");
    ostringstream out;
    Engine engine;
    engine.exec_main_loop(in, out);
    LIBBOARDGAME_CHECK_EQUAL(string("= \n\n"), out.str());
}

LIBBOARDGAME_TEST_CASE(gtp_engine_command_with_id)
{
    istringstream in("10 version\n");
    ostringstream out;
    Engine engine;
    engine.exec_main_loop(in, out);
    LIBBOARDGAME_CHECK_EQUAL(string("=10 \n\n"), out.str());
}

/** Check that invalid responses with one empty line are sanitized. */
LIBBOARDGAME_TEST_CASE(gtp_engine_empty_lines)
{
    istringstream in("invalid_response\n");
    ostringstream out;
    InvalidResponseEngine engine;
    engine.exec_main_loop(in, out);
    LIBBOARDGAME_CHECK_EQUAL(string("= This response is invalid\n"
                             " \n"
                             "because it contains an empty line\n"
                             "\n"),
                      out.str());
}

/** Check that invalid responses with two empty lines are sanitized. */
LIBBOARDGAME_TEST_CASE(gtp_engine_empty_lines_2)
{
    istringstream in("invalid_response_2\n");
    ostringstream out;
    InvalidResponseEngine engine;
    engine.exec_main_loop(in, out);
    LIBBOARDGAME_CHECK_EQUAL(string("= This response is invalid\n"
                             " \n"
                             " \n"
                             "because it contains two empty lines\n"
                             "\n"),
                      out.str());
}

LIBBOARDGAME_TEST_CASE(gtp_engine_unknown_command)
{
    istringstream in("unknowncommand\n");
    ostringstream out;
    Engine engine;
    engine.exec_main_loop(in, out);
    LIBBOARDGAME_CHECK(out.str().size() >= 2);
    LIBBOARDGAME_CHECK_EQUAL(string("? "), out.str().substr(0, 2));
}

//-----------------------------------------------------------------------------
