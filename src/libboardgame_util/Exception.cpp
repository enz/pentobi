//-----------------------------------------------------------------------------
/** @file Exception.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Exception.h"

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

Exception::Exception(const string& s)
    : m_s(s)
{
}

Exception::Exception(const format& f)
    : m_s(f.str())
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
