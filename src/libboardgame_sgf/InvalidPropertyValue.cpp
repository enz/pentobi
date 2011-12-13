//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/InvalidPropertyValue.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "InvalidPropertyValue.h"

namespace libboardgame_sgf {

using namespace std;
using boost::format;

//-----------------------------------------------------------------------------

InvalidPropertyValue::InvalidPropertyValue(const string& id,
                                           const string& value)
    : Exception(format("Invalid value '%1' for SGF property '%2%'")
                % value % id)
{
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
