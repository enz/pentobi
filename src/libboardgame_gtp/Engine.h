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
    typedef function<void(const Arguments&, Response&)> Handler;

    typedef function<void(Response&)> HandlerNoArgs;

    typedef function<void(const Arguments&)> HandlerNoResponse;

    typedef function<void()> HandlerNoArgsNoResponse;

    /** @page libboardgame_gtp_commands libboardgame_gtp::Engine GTP commands
        <dl>
        <dt>@link cmd_known_command() @c known_command @endlink</dt>
        <dd>@copydoc cmd_known_command() </dd>
        <dt>@link cmd_list_commands() @c list_commands @endlink</dt>
        <dd>@copydoc cmd_list_commands() </dd>
        <dt>@link cmd_name() @c name @endlink</dt>
        <dd>@copydoc cmd_name() </dd>
        <dt>@link cmd_protocol_version() @c protocol_version @endlink</dt>
        <dd>@copydoc cmd_protocol_version() </dd>
        <dt>@link cmd_quit() @c quit @endlink</dt>
        <dd>@copydoc cmd_quit() </dd>
        <dt>@link cmd_version() @c version @endlink</dt>
        <dd>@copydoc cmd_version() </dd>
        </dl> */
    /** @name Command handlers */
    /** @{ */
    void cmd_known_command(const Arguments&, Response&);
    void cmd_list_commands(Response&);
    void cmd_name(Response&);
    void cmd_protocol_version(Response&);
    void cmd_quit();
    void cmd_version(Response&);
    /** @} */ // @name

    Engine();

    Engine(const Engine&) = delete;

    Engine& operator=(const Engine&) const = delete;

    virtual ~Engine();

    /** Execute commands from an input stream.
        @param in The input stream
        @param throw_on_fail Whether to throw an exception if a command fails,
        or to continue executing the remainign commands
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
    void add(const string& name,
             void (T::*f)(const Arguments&, Response&), T* t);

    template<class T>
    void add(const string& name, void (T::*f)(const Arguments&), T* t);

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
    void add(const string& name, void (T::*f)(const Arguments&, Response&));

    template<class T>
    void add(const string& name, void (T::*f)(const Arguments&));

    template<class T>
    void add(const string& name, void (T::*f)(Response&));

    template<class T>
    void add(const string& name, void (T::*f)());

private:
    /** Mapping of command name to command handler.
        They key is a string subrange, not a string, to allow looking up the
        command name using Command::name_as_subrange() without creating a
        temporary string for the command name. The value of type CmdInfo with
        the name string and callback function are stored in an object allocated
        on the heap to ensure that the range stays valid, if the value object
        is copied. */
    typedef map<string, Handler> Handlers;


    /** Flag to quit main loop. */
    bool m_quit;

    Handlers m_handlers;


    bool handle_cmd(CmdLine& line, ostream* out, Response& response,
                    string& buffer);

    static void no_args_wrapper(const HandlerNoArgs& h,
                                const Arguments& args, Response& response);

    static void no_response_wrapper(const HandlerNoResponse& h,
                                    const Arguments& args, Response&);

    static void no_args_no_response_wrapper(const HandlerNoArgsNoResponse& h,
                                           const Arguments& args, Response&);
};

template<class T>
void Engine::add(const string& name, void (T::*f)(const Arguments&, Response&))
{
    add(name, f, dynamic_cast<T*>(this));
}

template<class T>
void Engine::add(const string& name, void (T::*f)(Response&))
{
    add(name, f, dynamic_cast<T*>(this));
}

template<class T>
void Engine::add(const string& name, void (T::*f)(const Arguments&))
{
    add(name, f, dynamic_cast<T*>(this));
}

template<class T>
void Engine::add(const string& name, void (T::*f)())
{
    add(name, f, dynamic_cast<T*>(this));
}

template<class T>
void Engine::add(const string& name,
                 void (T::*f)(const Arguments&, Response&), T* t)
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
void Engine::add(const string& name, void (T::*f)(const Arguments&), T* t)
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
