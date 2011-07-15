//-----------------------------------------------------------------------------
/** @file TeeStream.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "TeeStream.h"

#include <cstdio>

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

TeeStreamBuf::TeeStreamBuf(streambuf* buffer1, streambuf* buffer2)
    : m_buffer1(buffer1),
      m_buffer2(buffer2)
{
}

int TeeStreamBuf::overflow(int c)
{
    if (c == EOF)
        return ! EOF;
    else
    {
        int r1 = m_buffer1->sputc(char(c));
        int r2 = m_buffer2->sputc(char(c));
        return (r1 == EOF || r2 == EOF) ? EOF : c;
    }
}

int TeeStreamBuf::sync()
{
    int r1 = m_buffer1->pubsync();
    int r2 = m_buffer2->pubsync();
    return (r1 == 0 && r2 == 0) ? 0 : -1;
}

//-----------------------------------------------------------------------------

TeeStream::TeeStream(ostream& out1, ostream& out2)
  : ostream(&m_buffer),
    m_out1(out1),
    m_out2(out2),
    m_buffer(out1.rdbuf(), out2.rdbuf())
{
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util
