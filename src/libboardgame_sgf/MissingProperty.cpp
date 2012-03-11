//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/MissingProperty.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "MissingProperty.h"

namespace libboardgame_sgf {

using namespace std;

//-----------------------------------------------------------------------------

MissingProperty::MissingProperty(const string& message)
    : InvalidTree(format("Missing SGF property: %1%") % message)
{
}

MissingProperty::MissingProperty(const string& id, const string& message)
    : InvalidTree(format("Missing SGF property '%1%': %2%") % id % message)
{
}

MissingProperty::MissingProperty(const string& id, const format& message)
    : InvalidTree(format("Missing SGF property '%1%': %2%")
                  % id % str(message))
{
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
