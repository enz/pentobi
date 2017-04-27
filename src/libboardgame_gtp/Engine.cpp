//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/Engine.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Engine.h"

#include <cctype>
#include <iostream>
#include "CmdLine.h"

namespace libboardgame_gtp {

//-----------------------------------------------------------------------------

/** Utility functions. */
namespace {

/** Check, if line contains a command. */
bool is_cmd_line(const string& line)
{
    for (char c : line)
        if (! isspace(static_cast<unsigned char>(c)))
            return c != '#';
    return false;
}

/** Read next command from stream.
    @param in The input stream.
    @param[out] c The command (reused for efficiency)
    @return @c false on end-of-stream or read error. */
bool read_cmd(CmdLine& c, istream& in)
{
    string line;
    while (getline(in, line))
        if (is_cmd_line(line))
            break;
    if (! in.fail())
    {
        c.init(line);
        return true;
    }
    else
        return false;
}

} // namespace

//-----------------------------------------------------------------------------

Engine::Engine()
{
    add("known_command", &Engine::cmd_known_command);
    add("list_commands", &Engine::cmd_list_commands);
    add("name", &Engine::cmd_name);
    add("protocol_version", &Engine::cmd_protocol_version);
    add("quit", &Engine::cmd_quit);
    add("version", &Engine::cmd_version);
}

Engine::~Engine() = default;

void Engine::add(const string& name, const Handler& f)
{
    m_handlers[name] = f;
}

void Engine::add(const string& name, const HandlerNoArgs& f)
{
    add(name,
        Handler(bind(no_args_wrapper, f, placeholders::_1, placeholders::_2)));
}

void Engine::add(const string& name, const HandlerNoResponse& f)
{
    add(name, Handler(bind(no_response_wrapper, f,
                           placeholders::_1, placeholders::_2)));
}

void Engine::add(const string& name, const HandlerNoArgsNoResponse& f)
{
    add(name, Handler(bind(no_args_no_response_wrapper, f,
                           placeholders::_1, placeholders::_2)));
}

/** Return @c true if command is known, @c false otherwise. */
void Engine::cmd_known_command(const Arguments& args, Response& response)
{
    response.set(contains(args.get()) ? "true" : "false");
}

/** List all known commands. */
void Engine::cmd_list_commands(Response& response)
{
    for (auto& i : m_handlers)
        response << i.first << '\n';
}

/** Return name. */
void Engine::cmd_name(Response& response)
{
    response.set("Unknown");
}

/** Return protocol version. */
void Engine::cmd_protocol_version(Response& response)
{
    response.set("2");
}

/** Quit command loop. */
void Engine::cmd_quit()
{
    m_quit = true;
}

/** Return empty version string.
    The GTP standard says to return empty string, if no meaningful response
    is available. */
void Engine::cmd_version(Response&)
{
}

bool Engine::contains(const string& name) const
{
    return m_handlers.count(name) > 0;
}

bool Engine::exec(istream& in, bool throw_on_fail, ostream* log)
{
    string line;
    Response response;
    string buffer;
    CmdLine cmd;
    while (getline(in, line))
    {
        if (! is_cmd_line(line))
            continue;
        cmd.init(line);
        if (log)
            *log << cmd.get_line() << '\n';
        bool status = handle_cmd(cmd, log, response, buffer);
        if (! status && throw_on_fail)
        {
            ostringstream msg;
            msg << "executing '" << cmd.get_line() << "' failed";
            throw Failure(msg.str());
        }
    }
    return ! in.fail();
}

void Engine::exec_main_loop(istream& in, ostream& out)
{
    m_quit = false;
    CmdLine cmd;
    Response response;
    string buffer;
    while (! m_quit)
    {
        if (read_cmd(cmd, in))
            handle_cmd(cmd, &out, response, buffer);
        else
            break;
    }
}

/** Call the handler of a command and write its response.
    @param line The command
    @param out The output stream for the response
    @param response A reusable response instance to avoid memory allocation in
    each function call
    @param buffer A reusable string instance to avoid memory allocation in each
    function call */
bool Engine::handle_cmd(CmdLine& line, ostream* out, Response& response,
                        string& buffer)
{
    on_handle_cmd_begin();
    bool status = true;
    try
    {
        response.clear();
        auto pos = m_handlers.find(line.get_name());
        if (pos != m_handlers.end())
        {
            Arguments args(line);
            (pos->second)(args, response);
        }
        else
        {
            status = false;
            response << "unknown command (" << line.get_name() << ')';
        }
    }
    catch (const Failure& failure)
    {
        status = false;
        response.set(failure.what());
    }
    if (out)
    {
        *out << (status ? '=' : '?');
        line.write_id(*out);
        *out << ' ';
        response.write(*out, buffer);
        out->flush();
    }
    return status;
}

void Engine::no_args_wrapper(const HandlerNoArgs& h, const Arguments& args,
                             Response& response)
{
    args.check_empty();
    h(response);
}

void Engine::no_response_wrapper(const HandlerNoResponse& h, const Arguments& args,
                                 Response&)
{
    h(args);
}

void Engine::no_args_no_response_wrapper(const HandlerNoArgsNoResponse& h,
                                         const Arguments& args, Response&)
{
    args.check_empty();
    h();
}

void Engine::on_handle_cmd_begin()
{
    // Default implementation does nothing
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp
