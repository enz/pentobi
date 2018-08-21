//-----------------------------------------------------------------------------
/** @file twogtp/FdStream.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "FdStream.h"

#include <cstring>
#include <unistd.h>

//-----------------------------------------------------------------------------

namespace {

const size_t put_back = 1;

} // namespace

//-----------------------------------------------------------------------------

FdInBuf::FdInBuf(int fd, size_t buf_size)
    : m_fd(fd),
      m_buf(buf_size + put_back)
{
    auto end = &(*m_buf.begin()) + m_buf.size();
    setg(end, end, end);
}

FdInBuf::~FdInBuf() = default; // Non-inline to avoid GCC -Winline warning

auto FdInBuf::underflow() -> int_type
{
    if (gptr() < egptr())
        return traits_type::to_int_type(*gptr());
    auto base = &m_buf.front();
    auto start = base;
    if (eback() == base)
    {
        memmove(base, egptr() - put_back, put_back);
        start += put_back;
    }
    auto n = read(m_fd, start, m_buf.size() - (start - base));
    if (n <= 0)
        return traits_type::eof();
    setg(base, start, start + n);
    return traits_type::to_int_type(*gptr());
}

//-----------------------------------------------------------------------------

FdInStream::FdInStream(int fd)
    : istream(nullptr),
      m_buf(fd)
{
    rdbuf(&m_buf);
}

//-----------------------------------------------------------------------------

FdOutBuf::~FdOutBuf() = default; // Non-inline to avoid GCC -Winline warning

auto FdOutBuf::overflow(int_type c) -> int_type
{
    if (c != traits_type::eof())
    {
        char buffer[1];
        buffer[0] = static_cast<char>(c);
        if (write(m_fd, buffer, 1) != 1)
            return traits_type::eof();
    }
    return c;
}

streamsize FdOutBuf::xsputn(const char_type* s, streamsize count)
{
    return write(m_fd, s, count);
}

//-----------------------------------------------------------------------------

FdOutStream::FdOutStream(int fd)
    : ostream(nullptr),
      m_buf(fd)
{
    rdbuf(&m_buf);
}

//-----------------------------------------------------------------------------
