//-----------------------------------------------------------------------------
/** @file libboardgame_util/Exception.cpp
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Exception.h"

namespace libboardgame_util {

//-----------------------------------------------------------------------------

Exception::Exception(const string& s)
    : m_s(s)
{
}

Exception::~Exception() throw()
{
}

const char* Exception::what() const throw()
{
    return m_s.c_str();
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util
