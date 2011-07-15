//-----------------------------------------------------------------------------
/** @file InvalidPropertyValue.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "InvalidPropertyValue.h"

namespace libboardgame_sgf {

using namespace std;

//-----------------------------------------------------------------------------

InvalidPropertyValue::InvalidPropertyValue(const string& message)
    : Exception(format("Invalid value for SGF property: %1%") % message)
{
}

InvalidPropertyValue::InvalidPropertyValue(const string& id,
                                           const string& message)
    : Exception(format("Invalid value for SGF property '%1%': %2%")
                % id % message)
{
}

InvalidPropertyValue::InvalidPropertyValue(const string& id,
                                           const format& message)
    : Exception(format("Invalid value for SGF property '%1%': %2%")
                % id % str(message))
{
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
