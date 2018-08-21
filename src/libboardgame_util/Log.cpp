//-----------------------------------------------------------------------------
/** @file libboardgame_util/Log.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_DISABLE_LOG

//-----------------------------------------------------------------------------

#include "Log.h"

#include <iostream>

#if defined ANDROID || defined __ANDROID__
#include <android/log.h>
#endif

namespace libboardgame_util {

//-----------------------------------------------------------------------------

namespace {

#if defined ANDROID || defined __ANDROID__

class AndroidBuf
    : public streambuf
{
public:
    AndroidBuf();

protected:
    int_type overflow(int_type c) override;

    int sync() override;

private:
    static const unsigned buffer_size = 8192;

    char m_buffer[buffer_size];
};

AndroidBuf::AndroidBuf()
{
    setp(m_buffer, m_buffer + buffer_size - 1);
}

auto AndroidBuf::overflow(int_type c) -> int_type
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

} // namespace

//-----------------------------------------------------------------------------

ostream* _log_stream = nullptr;

//-----------------------------------------------------------------------------

void _log(const string& s)
{
    if (_log_stream == nullptr)
        return;
    if (s.empty())
        *_log_stream << '\n';
    else if (s.back() == '\n')
        *_log_stream << s;
    else
    {
        string line = s;
        line += '\n';
        *_log_stream << line;
    }
}

void _log_close()
{
#if defined ANDROID || defined __ANDROID__
    cerr.rdbuf(nullptr);
#endif
}

void _log_init()
{
#if defined ANDROID || defined __ANDROID__
    cerr.rdbuf(&android_buffer);
#endif
    _log_stream = &cerr;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

//-----------------------------------------------------------------------------

#endif // ! LIBBOARDGAME_DISABLE_LOG
