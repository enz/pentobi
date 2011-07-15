//-----------------------------------------------------------------------------
/** @file TeeStream.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_TEE_STREAM_H
#define LIBBOARDGAME_UTIL_TEE_STREAM_H

#include <iostream>

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

class TeeStreamBuf
    : public streambuf
{
public:
    TeeStreamBuf(streambuf* buffer1, streambuf* buffer2);

protected:
    int overflow(int c);

    int sync();

private:
    streambuf* m_buffer1;

    streambuf* m_buffer2;
};

//-----------------------------------------------------------------------------

class TeeStream
    : public ostream
{
public:
    TeeStream(ostream& out1, ostream& out2);

    ostream& get_out1();

    ostream& get_out2();

private:
    ostream& m_out1;

    ostream& m_out2;

    TeeStreamBuf m_buffer;
};

inline ostream& TeeStream::get_out1()
{
    return m_out1;
}

inline ostream& TeeStream::get_out2()
{
    return m_out2;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_TEE_STREAM_H
