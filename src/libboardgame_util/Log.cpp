//-----------------------------------------------------------------------------
/** @file libboardgame_util/Log.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Log.h"

#include <fstream>
#include <iostream>

#if defined ANDROID || defined __ANDROID__
#include <android/log.h>
#endif

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

namespace {

#if defined ANDROID || defined __ANDROID__

class AndroidBuf
    : public streambuf
{
public:
    AndroidBuf();

protected:
    int overflow(int c) override;

    int sync() override;

private:
    static const unsigned buffer_size = 8192;

    char m_buffer[buffer_size];
};

AndroidBuf::AndroidBuf()
{
    setp(m_buffer, m_buffer + buffer_size - 1);
}

int AndroidBuf::overflow(int c)
{
    if (c == traits_type::eof())
    {
        *pptr() = traits_type::to_char_type(c);
        sbumpc();
    }
    return sync() ? traits_type::eof(): traits_type::not_eof(c);
}

int AndroidBuf::sync()
{
    int n = 0;
    if (pbase() != pptr())
    {
        __android_log_print(ANDROID_LOG_INFO, "Native", "%s",
                            string(pbase(), pptr() - pbase()).c_str());
        n = 0;
        setp(m_buffer, m_buffer + buffer_size - 1);
    }
    return n;
}

AndroidBuf android_buffer;

#endif // defined(ANDROID) || defined(__ANDROID__)

ostream* log_stream = &cerr;

/** Unopened file stream serves as null stream. */
ofstream null_stream;

} // namespace

//-----------------------------------------------------------------------------

ostream& get_log()
{
    return *log_stream;
}

void log(const string& s)
{
    if (s.empty())
        *log_stream << '\n';
    else if (s.back() == '\n')
        *log_stream << s;
    else
    {
        string line = s;
        line += '\n';
        *log_stream << line;
    }
}

void log_close()
{
#if defined ANDROID || defined __ANDROID__
    cerr.rdbuf(nullptr);
#endif
}

void log_init()
{
#if defined ANDROID || defined __ANDROID__
    cerr.rdbuf(&android_buffer);
#endif
}

void set_log(ostream& out)
{
    log_stream = &out;
}

void set_log_null()
{
    set_log(null_stream);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util
