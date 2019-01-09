//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/Engine.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

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
        if (isspace(static_cast<unsigned char>(c)) == 0)
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
    return false;
}

} // namespace

//-----------------------------------------------------------------------------

Engine::Engine()
{
    add("known_command", &Engine::cmd_known_command);
    add("list_commands", &Engine::cmd_list_commands);
    add("quit", &Engine::cmd_quit);
}

Engine::~Engine() = default; // Non-inline to avoid GCC -Winline warning

void Engine::add(const string& name, const Handler& f)
{
    m_handlers[name] = f;
}

void Engine::add(const string& name, const HandlerNoArgs& f)
{
    add(name, [f](Arguments args, Response& response) {
        args.check_empty();
        f(response);
    });
}

void Engine::add(const string& name, const HandlerNoResponse& f)
{
    add(name, [f](Arguments args, Response&) {
        f(args);
    });
}

void Engine::add(const string& name, const HandlerNoArgsNoResponse& f)
{
    add(name, [f](Arguments args, Response&) {
        args.check_empty();
        f();
    });
}

/** Return @c true if command is known, @c false otherwise. */
void Engine::cmd_known_command(Arguments args, Response& response)
{
    response.set(contains(string(args.get())) ? "true" : "false");
}

/** List all known commands. */
void Engine::cmd_list_commands(Response& response)
{
    for (auto& i : m_handlers)
        response << i.first << '\n';
}

/** Quit command loop. */
void Engine::cmd_quit()
{
    m_quit = true;
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
        if (log != nullptr)
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
        auto pos = m_handlers.find(string(line.get_name()));
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
    if (out != nullptr)
    {
        *out << (status ? '=' : '?');
        line.write_id(*out);
        *out << ' ';
        response.write(*out, buffer);
        out->flush();
    }
    return status;
}

void Engine::on_handle_cmd_begin()
{
    // Default implementation does nothing
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp
