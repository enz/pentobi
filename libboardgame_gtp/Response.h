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
    /** Set response. */
    void set(const string& response) { m_stream.str(response); }

    void clear();

    /** Write response to output stream.
        Also sanitizes responses containing empty lines ("\n\n" cannot occur
        in a response, because it means end of response; it will be replaced by
        "\n \n") and adds "\n\n" add the end of the response. */
    void write(ostream& out, string& buffer) const;

    template<typename TYPE>
    Response& operator<<(const TYPE& t) { m_stream << t; return *this; }

private:
    /** Response stream */
    ostringstream m_stream;

    /** Dummy for restoring default format flags*/
    ios m_dummy{nullptr};
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp

#endif // LIBBOARDGAME_GTP_RESPONSE_H
