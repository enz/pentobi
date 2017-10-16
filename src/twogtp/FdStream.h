//-----------------------------------------------------------------------------
/** @file twogtp/FdStream.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef TWOGTP_FDSTREAM_H
#define TWOGTP_FDSTREAM_H

#include <iostream>
#include <vector>

using namespace std;

//-----------------------------------------------------------------------------

/** Input stream buffer from a file descriptor. */
class FdInBuf
    : public streambuf
{
public:
    explicit FdInBuf(int fd, size_t buf_size = 1024);

    ~FdInBuf() override;

protected:
    int_type underflow() override;

private:
    int m_fd;

    vector<char_type> m_buf;
};

//-----------------------------------------------------------------------------

/** Input stream from a file descriptor. */
class FdInStream final
    : public istream
{
public:
    explicit FdInStream(int fd);

private:
    FdInBuf m_buf;
};

//-----------------------------------------------------------------------------

/** Output stream buffer from a file descriptor. */
class FdOutBuf
    : public streambuf
{
public:
    explicit FdOutBuf(int fd)
        : m_fd(fd)
    { }

    ~FdOutBuf() override;

protected:
    int_type overflow(int_type c) override;

    streamsize xsputn(const char_type* s, streamsize count) override;

private:
    int m_fd;
};

//-----------------------------------------------------------------------------

/** Output stream from a file descriptor. */
class FdOutStream final
    : public ostream
{
public:
    explicit FdOutStream(int fd);

private:
    FdOutBuf m_buf;
};

//-----------------------------------------------------------------------------

#endif // TWOGTP_FDSTREAM_H
