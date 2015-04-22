//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/Response.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_GTP_RESPONSE_H
#define LIBBOARDGAME_GTP_RESPONSE_H

#include <sstream>
#include <string>

namespace libboardgame_gtp {

using namespace std;

//-----------------------------------------------------------------------------

class Response
{
public:
    ~Response();

    /** Conversion to output stream.
        Returns reference to response stream. */
    operator ostream&();

    /** Get response.
        @return A copy of the internal response string stream */
    string to_string() const;

    /** Set response. */
    void set(const string& response);

    void clear();

    /** Write response to output stream.
        Also sanitizes responses containing empty lines ("\n\n" cannot occur
        in a response, because it means end of response; it will be replaced by
        "\n \n") and adds "\n\n" add the end of the response. */
    void write(ostream& out, string& buffer) const;

private:
    /** Dummy stream for copying default formatting settings. */
    static ostringstream s_dummy;

    /** Response stream */
    ostringstream m_stream;
};

inline Response::operator ostream&()
{
    return m_stream;
}

inline void Response::clear()
{
    m_stream.str("");
    m_stream.copyfmt(s_dummy);
}

inline string Response::to_string() const
{
    return m_stream.str();
}

inline void Response::set(const string& response)
{
    m_stream.str(response);
}

//-----------------------------------------------------------------------------

/** @relates libboardgame_gtp::Response */
template<typename TYPE>
inline Response& operator<<(Response& r, const TYPE& t)
{
    static_cast<ostream&>(r) << t;
    return r;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp

#endif // LIBBOARDGAME_GTP_RESPONSE_H
