//-----------------------------------------------------------------------------
/** @file twogtp/GtpConnection.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef TWOGTP_GTP_CONNECTION_H
#define TWOGTP_GTP_CONNECTION_H

#if ! defined __GNUC__ || defined __clang__ || ! HAVE_UNISTD_H
#error "GtpConnection currently only implemented for Posix/GCC"
#endif

#include <iosfwd>
#include <memory>
#include <string>
#include <ext/stdio_filebuf.h> // GCC
#include "libboardgame_util/Exception.h"

using namespace std;
using libboardgame_util::Exception;

//-----------------------------------------------------------------------------

/** Invokes a GTP engine in an external process. */
class GtpConnection
{
public:
    class Failure
        : public Exception
    {
    public:
        Failure(const string& message)
            : Exception(message)
        { }
    };

    GtpConnection(const string& command);

    ~GtpConnection();

    void enable_log(const string& prefix = "");

    /** Send a GTP command.
        @param command The command.
        @return The response if the command returns a success status.
        @throws Failure If the command returns an error status. */
    string send(const string& command);

private:
    bool m_quiet;

    string m_prefix;

    unique_ptr<__gnu_cxx::stdio_filebuf<char>> m_buf_in;

    unique_ptr<__gnu_cxx::stdio_filebuf<char>> m_buf_out;

    unique_ptr<istream> m_in;

    unique_ptr<ostream> m_out;
};

//-----------------------------------------------------------------------------

#endif // TWOGTP_GTP_CONNECTION_H
