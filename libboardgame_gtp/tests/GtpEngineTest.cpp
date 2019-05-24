//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/tests/GtpEngineTest.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "libboardgame_gtp/GtpEngine.h"
#include "libboardgame_test/Test.h"

using namespace std;
using namespace libboardgame_gtp;

//-----------------------------------------------------------------------------

namespace {

//-----------------------------------------------------------------------------

/** GTP engine returning invalid responses for testing class Engine.
    For testing that the base class Engine sanitizes responses of
    subclasses that contain empty lines (see
    @ref libboardgame_gtp::GtpEngine::exec_main_loop). */
class InvalidResponseEngine
    : public GtpEngine
{
public:
    InvalidResponseEngine();

    void invalid_response(Response& r);

    void invalid_response_2(Response& r);
};

InvalidResponseEngine::InvalidResponseEngine()
{
    add("invalid_response", &InvalidResponseEngine::invalid_response);
    add("invalid_response_2", &InvalidResponseEngine::invalid_response_2);
}

void InvalidResponseEngine::invalid_response(Response& r)
{
    r << "This response is invalid\n"
      << "\n"
      << "because it contains an empty line";
}

void InvalidResponseEngine::invalid_response_2(Response& r)
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
    istringstream in("known_command known_command\n");
    ostringstream out;
    GtpEngine engine;
    engine.exec_main_loop(in, out);
    LIBBOARDGAME_CHECK_EQUAL(string("= true\n\n"), out.str());
}

LIBBOARDGAME_TEST_CASE(gtp_engine_command_with_id)
{
    istringstream in("10 known_command known_command\n");
    ostringstream out;
    GtpEngine engine;
    engine.exec_main_loop(in, out);
    LIBBOARDGAME_CHECK_EQUAL(string("=10 true\n\n"), out.str());
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
    GtpEngine engine;
    engine.exec_main_loop(in, out);
    LIBBOARDGAME_CHECK(out.str().size() >= 2);
    LIBBOARDGAME_CHECK_EQUAL(string("? "), out.str().substr(0, 2));
}

//-----------------------------------------------------------------------------
