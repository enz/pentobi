//-----------------------------------------------------------------------------
/** @file libboardgame_util/Exception.h
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_EXCEPTION_H
#define LIBBOARDGAME_UTIL_EXCEPTION_H

#include <exception>
#include <string>

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

class Exception
    : public exception
{
public:
    Exception(const string& s);

    virtual ~Exception() throw();

    const char* what() const throw() override;

private:
    string m_s;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_EXCEPTION_H
