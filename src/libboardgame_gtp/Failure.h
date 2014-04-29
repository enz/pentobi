//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/Failure.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_GTP_FAILURE_H
#define LIBBOARDGAME_GTP_FAILURE_H

#include <string>

namespace libboardgame_gtp {

using namespace std;

//-----------------------------------------------------------------------------

/** GTP failure.
    Command handlers generate a GTP error response by throwing an instance
    of Failure. */
class Failure
{
public:
    Failure(const string& response = "");

    ~Failure();

    string get_response() const;

private:
    string m_response;
};

inline Failure::Failure(const string& response)
    : m_response(response)
{
}

inline Failure::~Failure()
{
}

inline string Failure::get_response() const
{
    return m_response;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp

#endif // LIBBOARDGAME_GTP_ENGINE_H
