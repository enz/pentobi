//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/Engine.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_GTP_ENGINE_H
#define LIBBOARDGAME_GTP_ENGINE_H

#include <functional>
#include <iosfwd>
#include <map>
#include "Arguments.h"
#include "Response.h"

namespace libboardgame_gtp {

class CmdLine;

using namespace std;

//-----------------------------------------------------------------------------

/** Base class for GTP engines.
    Commands can be added with Engine::add(). Existing commands can be
    overridden by registering a new handler for the command.
    @see @ref libboardgame_gtp_commands */
class Engine
{
public:
    using Handler = function<void(Arguments, Response&)>;

    using HandlerNoArgs = function<void(Response&)>;

    using HandlerNoResponse = function<void(Arguments)>;

    using HandlerNoArgsNoResponse = function<void()>;


    /** @page libboardgame_gtp_commands libboardgame_gtp::Engine GTP commands
        <dl>
        <dt>@link cmd_known_command() @c known_command @endlink</dt>
        <dd>@copydoc cmd_known_command() </dd>
        <dt>@link cmd_list_commands() @c list_commands @endlink</dt>
        <dd>@copydoc cmd_list_commands() </dd>
        <dt>@link cmd_quit() @c quit @endlink</dt>
        <dd>@copydoc cmd_quit() </dd>
        </dl> */
    /** @name Command handlers */
    /** @{ */
    void cmd_known_command(Arguments args, Response& response);
    void cmd_list_commands(Response& response);
    void cmd_quit();
    /** @} */ // @name

    Engine();

    Engine(const Engine&) = delete;

    Engine& operator=(const Engine&) const = delete;

    virtual ~Engine();

    /** Execute commands from an input stream.
        @param in The input stream
        @param throw_on_fail Whether to throw an exception if a command fails,
        or to continue executing the remaining commands
        @param log Stream for logging the commands and responses to.
        @return The stream state as a bool
        @throws Failure If a command fails, and @c throw_on_fail is @c true */
    bool exec(istream& in, bool throw_on_fail, ostream* log);

    /** Run the main command loop.
        Reads lines from input stream, calls the corresponding command handler
        and writes the response to the output stream. Empty lines in the
        command responses will be replaced by a line containing a single space,
        because empty lines are not allowed in GTP responses. */
    void exec_main_loop(istream& in, ostream& out);

    /** Register command handler.
        If a command was already registered with the same name, it will be
        replaced by the new command. */
    void add(const string& name, const Handler& f);

    void add(const string& name, const HandlerNoArgs& f);

    void add(const string& name, const HandlerNoResponse& f);

    void add(const string& name, const HandlerNoArgsNoResponse& f);

    /** Register a member function as a command handler.
        If a command was already registered with the same name, it will be
        replaced by the new command. */
    template<class T>
    void add(const string& name, void (T::*f)(Arguments, Response&), T* t);

    template<class T>
    void add(const string& name, void (T::*f)(Arguments), T* t);

    template<class T>
    void add(const string& name, void (T::*f)(Response&), T* t);

    template<class T>
    void add(const string& name, void (T::*f)(), T* t);

    /** Returns if command registered. */
    bool contains(const string& name) const;

protected:
    /** Hook function to be executed before each command.
        The default implementation does nothing. */
    virtual void on_handle_cmd_begin();

    /** Register a member function of the current instance as a command
        handler.
        If a command was already registered with the same name, it will be
        replaced by the new command. */
    template<class T>
    void add(const string& name, void (T::*f)(Arguments, Response&));

    template<class T>
    void add(const string& name, void (T::*f)(Arguments));

    template<class T>
    void add(const string& name, void (T::*f)(Response&));

    template<class T>
    void add(const string& name, void (T::*f)());

private:
    /** Flag to quit main loop. */
    bool m_quit;

    map<string, Handler> m_handlers;


    bool handle_cmd(CmdLine& line, ostream* out, Response& response,
                    string& buffer);
};

template<class T>
void Engine::add(const string& name, void (T::*f)(Arguments, Response&))
{
    add(name, f, dynamic_cast<T*>(this));
}

template<class T>
void Engine::add(const string& name, void (T::*f)(Response&))
{
    add(name, f, dynamic_cast<T*>(this));
}

template<class T>
void Engine::add(const string& name, void (T::*f)(Arguments))
{
    add(name, f, dynamic_cast<T*>(this));
}

template<class T>
void Engine::add(const string& name, void (T::*f)())
{
    add(name, f, dynamic_cast<T*>(this));
}

template<class T>
void Engine::add(const string& name, void (T::*f)(Arguments, Response&), T* t)
{
    assert(f);
    add(name,
        static_cast<Handler>(bind(f, t, placeholders::_1, placeholders::_2)));
}

template<class T>
void Engine::add(const string& name, void (T::*f)(Response&), T* t)
{
    assert(f);
    add(name, static_cast<HandlerNoArgs>(bind(f, t, placeholders::_1)));
}

template<class T>
void Engine::add(const string& name, void (T::*f)(Arguments), T* t)
{
    assert(f);
    add(name, static_cast<HandlerNoResponse>(bind(f, t, placeholders::_1)));
}

template<class T>
void Engine::add(const string& name, void (T::*f)(), T* t)
{
    assert(f);
    add(name, static_cast<HandlerNoArgsNoResponse>(bind(f, t)));
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp

#endif // LIBBOARDGAME_GTP_ENGINE_H
