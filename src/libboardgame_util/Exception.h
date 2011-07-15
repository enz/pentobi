//-----------------------------------------------------------------------------
/** @file Exception.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_EXCEPTION_H
#define LIBBOARDGAME_UTIL_EXCEPTION_H

#include <exception>
#include <string>
#include <boost/format.hpp>

namespace libboardgame_util {

using namespace std;
using boost::format;

//-----------------------------------------------------------------------------

class Exception
    : public exception
{
public:
    Exception(const string& s);

    Exception(const format& f);

    virtual ~Exception() throw();

    const char* what() const throw();

private:
    string m_s;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_EXCEPTION_H
