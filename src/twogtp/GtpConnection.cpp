//-----------------------------------------------------------------------------
/** @file twogtp/GtpConnection.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "GtpConnection.h"

#include <cstdlib>
#include <cstring>
#include <vector>
#include <unistd.h>
#include "FdStream.h"
#include "libboardgame_util/Log.h"

using libboardgame_util::log;

//-----------------------------------------------------------------------------

namespace {

void terminate_child(const string& message)
{
    log(message);
    exit(1);
}

vector<string> split_args(string s)
{
    vector<string> result;
    bool escape = false;
    bool is_in_string = false;
    ostringstream token;
    for (size_t i = 0; i < s.size(); ++i)
    {
        char c = s[i];
        if (c == '"' && ! escape)
        {
            if (is_in_string)
            {
                result.push_back(token.str());
                token.str("");
            }
            is_in_string = ! is_in_string;
        }
        else if (isspace(c) && ! is_in_string)
        {
            if (! token.str().empty())
            {
                result.push_back(token.str());
                token.str("");
            }
        }
        else
            token << c;
        escape = (c == '\\' && ! escape);
    }
    if (! token.str().empty())
        result.push_back(token.str());
    return result;
}

} // namespace

//-----------------------------------------------------------------------------

GtpConnection::GtpConnection(const string& command)
    : m_quiet(true)
{
    vector<string> args = split_args(command);
    if (args.size() == 0)
        throw Exception("GtpConnection: empty command line");
    int fd1[2];
    if (pipe(fd1) < 0)
        throw Exception("GtpConnection: pipe creation failed");
    int fd2[2];
    if (pipe(fd2) < 0)
    {
        close(fd1[0]);
        close(fd1[1]);
        throw Exception("GtpConnection: pipe creation failed");
    }
    pid_t pid;
    if ((pid = fork()) < 0)
        throw Exception("GtpConnection: fork failed");
    else if (pid > 0) // Parent
    {
        close(fd1[0]);
        close(fd2[1]);
        m_in.reset(new FdInStream(fd2[0]));
        m_out.reset(new FdOutStream(fd1[1]));
        return;
    }
    else // Child
    {
        close(fd1[1]);
        close(fd2[0]);
        if (fd1[0] != STDIN_FILENO)
            if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
            {
                close(fd1[0]);
                terminate_child("GtpConnection: dup2 to stdin failed");
            }
        if (fd2[1] != STDOUT_FILENO)
            if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
            {
                close(fd2[1]);
                terminate_child("GtpConnection: dup2 to stdout failed");
            }
        char** const argv = new char*[args.size() + 1];
        for (size_t i = 0; i < args.size(); ++i)
        {
            argv[i] = new char[args[i].size()];
            strcpy(argv[i], args[i].c_str());
        }
        argv[args.size()] = 0;
        if (execvp(args[0].c_str(), argv) == -1)
            terminate_child("Could not execute '" + command + "'");
    }
}

GtpConnection::~GtpConnection()
{
}

void GtpConnection::enable_log(const string& prefix)
{
    m_quiet = false;
    m_prefix = prefix;
}

string GtpConnection::send(const string& command)
{
    if (! m_quiet)
        log(m_prefix, ">> ", command);
    *m_out << command << '\n';
    m_out->flush();
    if (! *m_out)
        throw Failure("GtpConnection: write failure");
    ostringstream response;
    bool done = false;
    bool is_first = true;
    bool success = true;
    while (! done)
    {
        string line;
        getline(*m_in, line);
        if (! *m_in)
            throw Failure("GtpConnection: read failure");
        if (! m_quiet && ! line.empty())
            log(m_prefix, "<< ", line);
        if (is_first)
        {
            if (line.size() < 2 || (line[0] != '=' && line[0] != '?')
                    || line[1] != ' ')
                throw Failure("GtpConnection: malformed response: '" + line
                              + "'");
            if (line[0] == '?')
                success = false;
            line = line.substr(2);
            response << line;
            is_first = false;
        }
        else
        {
            if (line.empty())
                done = true;
            else
                response << '\n' << line;
        }
    }
    if (! success)
        throw Failure(response.str());
    return response.str();
}

//-----------------------------------------------------------------------------
