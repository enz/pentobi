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
    : InvalidTree("Missing SGF property: " + message)
{
}

MissingProperty::MissingProperty(const string& id, const string& message)
    : InvalidTree("Missing SGF property '" + id + ": " + message)
{
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf
