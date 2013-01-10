//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/Failure.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_GTP_FAILURE_H
#define LIBBOARDGAME_GTP_FAILURE_H

#include <string>
#include <boost/format.hpp>

namespace libboardgame_gtp {

using namespace std;
using boost::format;

//-----------------------------------------------------------------------------

/** GTP failure.
    Command handlers generate a GTP error response by throwing an instance
    of Failure. */
class Failure
{
public:
    Failure(const string& response = "");

    Failure(const format& response);

    ~Failure() throw();

    string get_response() const;

private:
    string m_response;
};

inline Failure::Failure(const string& response)
    : m_response(response)
{
}

inline Failure::Failure(const format& response)
    : m_response(response.str())
{
}

inline Failure::~Failure() throw()
{
}

inline string Failure::get_response() const
{
    return m_response;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp

#endif // LIBBOARDGAME_GTP_ENGINE_H
