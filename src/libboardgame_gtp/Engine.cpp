//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/Engine.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Engine.h"

#include <cassert>
#include <cctype>
#include <fstream>
#include <iostream>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

namespace libboardgame_gtp {

using namespace std;
using boost::barrier;
using boost::condition_variable;
using boost::lock_guard;
using boost::mutex;
using boost::thread;
using boost::unique_lock;
using boost::algorithm::starts_with;

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

namespace {

/** Ponder thread used by Engine::MainLoop().
    This thread calls Engine::ponder() while the engine is waiting for the
    next command.
    @see Engine::ponder() */
class PonderThread
{
public:
    PonderThread(Engine& engine);

    ~PonderThread() throw();

    void start_ponder();

    void stop_ponder();

private:
    class Function
    {
    public:
        Function(PonderThread& t);

        void operator()();

    private:
        PonderThread& m_thread;
    };

    friend class PonderThread::Function;

    bool m_quit;

    Engine& m_engine;

    barrier m_ready;

    mutex m_start_ponder_mutex;

    mutex m_ponder_finished_mutex;

    condition_variable m_start_ponder;

    condition_variable m_ponder_finished;

    unique_lock<mutex> m_ponder_finished_lock;

    /** The thread to run the ponder function.
        Order dependency: must be constructed as the last member, because the
        constructor starts the thread. */
    thread m_thread;
};

PonderThread::Function::Function(PonderThread& t)
    : m_thread(t)
{
}

void PonderThread::Function::operator()()
{
    unique_lock<mutex> lock(m_thread.m_start_ponder_mutex);
    m_thread.m_ready.wait();
    m_thread.m_quit = false;
    while (true)
    {
        m_thread.m_start_ponder.wait(lock);
        auto& engine = m_thread.m_engine;
        if (! m_thread.m_quit)
        {
            engine.ponder();
            {
                lock_guard<mutex> lock(m_thread.m_ponder_finished_mutex);
                m_thread.m_ponder_finished.notify_all();
            }
        }
        else
            return;
    }
}

PonderThread::PonderThread(Engine& engine)
    : m_engine(engine),
      m_ready(2),
      m_ponder_finished_lock(m_ponder_finished_mutex),
      m_thread(Function(*this))
{
    m_ready.wait();
}

PonderThread::~PonderThread() throw()
{
    m_quit = true;
    {
        lock_guard<mutex> lock(m_start_ponder_mutex);
        m_start_ponder.notify_all();
    }
    m_thread.join();
}

void PonderThread::start_ponder()
{
    m_engine.init_ponder();
    {
        lock_guard<mutex> lock(m_start_ponder_mutex);
        m_start_ponder.notify_all();
    }
}

void PonderThread::stop_ponder()
{
    m_engine.stop_ponder();
    m_ponder_finished.wait(m_ponder_finished_lock);
}

/** Thread for reading the next command line.
    This thread is used instead of the simple function read_command(CmdLine&),
    if Engine is compiled with interrupt support.
    @see Engine::interrupt() */
class ReadThread
{
public:
    ReadThread(istream& in, Engine& engine);

    ~ReadThread() throw();

    bool read_cmd(CmdLine& c);

private:
    /** Data used by ReadThread and ReadThread::Function.
        The lifetime of this data must exceed the lifetime of both the
        ReadThread object and the thread function. There are use cases in which
        the user wants to destruct the ReadThread object before the EOF of the
        input stream is reached (e.g. if a command handler throws an exception
        other than Failure), and we cannot terminate the thread if it is doing
        a blocking read on the input stream. Therefore the Data object is
        allocated on the heap and uses a thread-safe reference counter to
        determine when it will be deleted. Note that it could be never
        deleted if the program exits before the EOF of the blocking input
        stream is used. Therefore this struct should not contain any members
        whose destructor is important to run.  */
    struct Data
    {
        int ref_count;

        bool quit;

        mutex ref_count_mutex;

        istream& in;

        Engine& engine;

        bool is_stream_good;

        barrier ready;

        mutex wait_cmd_mutex;

        condition_variable wait_cmd;

        mutex cmd_received_mutex;

        condition_variable cmd_received;

        unique_lock<mutex> cmd_received_lock;

        CmdLine cmd;

        /** Increment reference count.
         */
        void inc_ref_count();

        /** Decrement reference count.
            @return true, if reference count is zero after the decrement */
        bool dec_ref_count();

        Data(istream& in, Engine& engine);
    };

    struct DataRef
    {
        Data& data;

        DataRef(Data& data);

        DataRef(const DataRef& data_ref);

        ~DataRef() throw();
    };

    class Function
    {
    public:
        Function(Data& data);

        void operator()();

    private:
        DataRef m_data_ref;
    };

    friend class ReadThread::Function;

    DataRef m_data_ref;

    /** The thread to run the read command function.
        Order dependency: must be constructed as the last member, because the
        constructor starts the thread. */
    thread m_thread;
};

ReadThread::Data::Data(istream& in, Engine& engine)
    : ref_count(0),
      quit(false),
      in(in),
      engine(engine),
      ready(2),
      cmd_received_lock(cmd_received_mutex)
{
}

void ReadThread::Data::inc_ref_count()
{
    lock_guard<mutex> lock(ref_count_mutex);
    ++ref_count;
}

bool ReadThread::Data::dec_ref_count()
{
    bool is_last_ref;
    {
        lock_guard<mutex> lock(ref_count_mutex);
        is_last_ref = ((--ref_count) == 0);
    }
    return is_last_ref;
}

ReadThread::DataRef::DataRef(Data& data)
    : data(data)
{
    data.inc_ref_count();
}

ReadThread::DataRef::DataRef(const DataRef& data_ref)
    : data(data_ref.data)
{
    data.inc_ref_count();
}

ReadThread::DataRef::~DataRef() throw()
{
    if (data.dec_ref_count())
        delete &data;
}

ReadThread::Function::Function(Data& data)
    : m_data_ref(data)
{
}

void ReadThread::Function::operator()()
{
    auto& data = m_data_ref.data;
    unique_lock<mutex> lock(data.wait_cmd_mutex);
    data.ready.wait();
    auto& engine = data.engine;
    auto& in = data.in;
    string line;
    data.is_stream_good = true;
    while (true)
    {
        while (getline(in, line))
        {
            if (data.quit)
                return;
            if (starts_with(line, "# interrupt"))
                engine.interrupt();
            else if (is_cmd_line(line))
                break;
        }
        data.wait_cmd.wait(lock);
        if (data.quit)
            return;
        if (! in.fail())
            data.cmd.init(line);
        else
            data.is_stream_good = false;
        {
            lock_guard<mutex> lock(data.cmd_received_mutex);
            data.cmd_received.notify_all();
        }
        if (in.fail())
            return;
    }
}

ReadThread::ReadThread(istream& in, Engine& engine)
    : m_data_ref(*(new Data(in, engine))),
      m_thread(Function(m_data_ref.data))
{
    m_data_ref.data.ready.wait();
    m_thread.detach();
}

ReadThread::~ReadThread() throw()
{
    auto& data = m_data_ref.data;
    data.quit = true;
    if (data.is_stream_good)
        // User destructs ReadThread before EOF was reached, so we cannot wait
        // for the read thread to finish because it could do a blocking read.
        return;
    m_thread.join();
}

bool ReadThread::read_cmd(CmdLine& c)
{
    auto& data = m_data_ref.data;
    {
        lock_guard<mutex> lock(data.wait_cmd_mutex);
        data.wait_cmd.notify_all();
    }
    data.cmd_received.wait(data.cmd_received_lock);
    if (! data.is_stream_good)
        return false;
    c.init(data.cmd);
    return true;
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

Engine::~Engine() throw()
{
}

void Engine::add(const string& name, Handler f)
{
    m_handlers[name] = f;
}

void Engine::add(const string& name, HandlerNoArgs f)
{
    add(name,
        Handler(bind(no_args_wrapper, f, placeholders::_1, placeholders::_2)));
}

void Engine::add(const string& name, HandlerNoResponse f)
{
    add(name, Handler(bind(no_response_wrapper, f,
                           placeholders::_1, placeholders::_2)));
}

void Engine::add(const string& name, HandlerNoArgsNoResponse f)
{
    add(name, Handler(bind(no_args_no_response_wrapper, f,
                           placeholders::_1, placeholders::_2)));
}

void Engine::add(const string& name, void (*f)(const Arguments&, Response&))
{
    add(name, Handler(f));
}

void Engine::add(const string& name, void (*f)(Response&))
{
    add(name, HandlerNoArgs(f));
}

void Engine::add(const string& name, void (*f)(const Arguments&))
{
    add(name, HandlerNoResponse(f));
}

void Engine::add(const string& name, void (*f)())
{
    add(name, HandlerNoArgsNoResponse(f));
}

/** Return @c true if command is known, @c false otherwise. */
void Engine::cmd_known_command(const Arguments& args, Response& response)
{
    bool is_known = (m_handlers.find(args.get()) != m_handlers.end());
    response.set(is_known ? "true" : "false");
}

/** List all known commands. */
void Engine::cmd_list_commands(Response& response)
{
    for (HandlerIterator i = m_handlers.begin(); i != m_handlers.end(); ++i)
        response << i->first << '\n';
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
    The GTP standard says to return empty string, if no meaningful reponse
    is available. */
void Engine::cmd_version(Response&)
{
}

bool Engine::contains(const string& name) const
{
    return (m_handlers.find(name) != m_handlers.end());
}

void Engine::remove(const string& name)
{
    auto i = m_handlers.find(name);
    if (i != m_handlers.end())
        m_handlers.erase(i);
}

string Engine::exec(const string& line)
{
    assert(is_cmd_line(line));
    Response response;
    string buffer;
    CmdLine cmd;
    cmd.init(line);
    ofstream null_stream;
    bool status = handle_cmd(cmd, null_stream, response, buffer);
    if (! status)
        throw Failure(response.to_string());
    return response.to_string();
}

bool Engine::exec(istream& in, bool throw_on_fail, ostream& log)
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
        log << cmd.get_line() << '\n';
        bool status = handle_cmd(cmd, log, response, buffer);
        if (! status && throw_on_fail)
            throw Failure(format("executing '%1%' failed") % cmd.get_line());
    }
    return ! in.fail();
}

void Engine::exec_main_loop(istream& in, ostream& out)
{
    // Tying of input to output stream (like used by std::cin/cout) is not
    // needed by this class and potentially harmful if threads are enabled and
    // the standard library implementation does not support simultaneous writes
    // to output stream by multiple threads
    in.tie(nullptr);
    m_quit = false;
    PonderThread ponder_thread(*this);
    ReadThread read_thread(in, *this);
    CmdLine cmd;
    Response response;
    string buffer;
    while (! m_quit)
    {
        ponder_thread.start_ponder();
        bool is_stream_good = read_thread.read_cmd(cmd);
        ponder_thread.stop_ponder();
        if (is_stream_good)
            handle_cmd(cmd, out, response, buffer);
        else
            m_quit = true;
    }
}

void Engine::exec_main_loop_st(istream& in, ostream& out)
{
    in.tie(nullptr); // See comment in exec_main_loop()
    m_quit = false;
    CmdLine cmd;
    Response response;
    string buffer;
    while (! m_quit)
    {
        if (read_cmd(cmd, in))
            handle_cmd(cmd, out, response, buffer);
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
bool Engine::handle_cmd(CmdLine& line, ostream& out, Response& response,
                        string& buffer)
{
    on_handle_cmd_begin();
    bool status = true;
    try
    {
        response.clear();
        HandlerIterator pos = m_handlers.find(line.get_name());
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
        response.set(failure.get_response());
    }
    // Keep output to cerr in sync, because out will be explicitely flushed by
    // this function after the response was written, but cerr could be buffered
    cerr.flush();
    out << (status ? '=' : '?');
    line.write_id(out);
    out << ' ';
    response.write(out, buffer);
    out << flush;
    return status;
}

void Engine::init_ponder()
{
    // Default implementation does nothing
}

void Engine::interrupt()
{
    // Default implementation does nothing
}

void Engine::no_args_wrapper(HandlerNoArgs h, const Arguments& args,
                             Response& response)
{
    args.check_empty();
    h(response);
}

void Engine::no_response_wrapper(HandlerNoResponse h, const Arguments& args,
                                 Response&)
{
    h(args);
}

void Engine::no_args_no_response_wrapper(HandlerNoArgsNoResponse h,
                                         const Arguments& args, Response&)
{
    args.check_empty();
    h();
}

void Engine::on_handle_cmd_begin()
{
    // Default implementation does nothing
}

void Engine::ponder()
{
    // Default implementation does nothing
}

void Engine::stop_ponder()
{
    // Default implementation does nothing
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp
