//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/InvalidPropertyValue.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_INVALID_PROPERTY_VALUE_H
#define LIBBOARDGAME_SGF_INVALID_PROPERTY_VALUE_H

#include "libboardgame_util/Exception.h"

namespace libboardgame_sgf {

using namespace std;
using libboardgame_util::Exception;

//-----------------------------------------------------------------------------

class InvalidPropertyValue
    : public Exception
{
public:
    InvalidPropertyValue(const string& id, const string& value);
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_INVALID_PROPERTY_VALUE_H
