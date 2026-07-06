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
    auto end = m_buf.data() + m_buf.size();
    setg(end, end, end);
}

FdInBuf::~FdInBuf() = default; // Non-inline to avoid GCC -Winline warning

auto FdInBuf::underflow() -> int_type
{
    if (gptr() < egptr())
        return traits_type::to_int_type(*gptr());
    auto base = m_buf.data();
    auto start = base;
    if (eback() == base)
    {
        memmove(base, egptr() - put_back, put_back);
        start += put_back;
    }
    ssize_t n;
    while (true)
    {
        n = read(m_fd, start, m_buf.size() - (start - base));
        if (n >= 0)
            break;
        if (errno == EINTR)
            continue;
        return traits_type::eof();
    }
    if (n == 0)
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
    if (c == traits_type::eof())
        return traits_type::not_eof(c);
    auto ch = traits_type::to_char_type(c);
    while (true)
    {
        auto n = write(m_fd, &ch, 1);
        if (n > 0)
            return c;
        if (n < 0 && errno != EINTR)
            return traits_type::eof();
    }
}

streamsize FdOutBuf::xsputn(const char_type* s, streamsize count)
{
    if (count <= 0)
        return 0;
    streamsize total = 0;
    while (total < count)
    {
        auto n = write(m_fd, s + total, static_cast<size_t>(count - total));
        if (n > 0)
            total += n;
        else if (n == 0)
            break;
        else
        {
            if (errno == EINTR)
                continue;
            break;
        }
    }
    return total;
}

//-----------------------------------------------------------------------------

FdOutStream::FdOutStream(int fd)
    : ostream(nullptr),
      m_buf(fd)
{
    rdbuf(&m_buf);
}

//-----------------------------------------------------------------------------
