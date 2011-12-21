//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/InvalidPropertyValue.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_INVALID_PROPERTY_VALUE_H
#define LIBBOARDGAME_SGF_INVALID_PROPERTY_VALUE_H

#include "libboardgame_util/Exception.h"

namespace libboardgame_sgf {

using namespace std;
using boost::format;
using libboardgame_util::Exception;

//-----------------------------------------------------------------------------

class InvalidPropertyValue
    : public Exception
{
public:
    template<typename T>
    InvalidPropertyValue(const string& id, const T& value);

    ~InvalidPropertyValue() throw();
};

template<typename T>
InvalidPropertyValue::InvalidPropertyValue(const string& id, const T& value)
    : Exception(format("Invalid value '%1%' for SGF property '%2%'")
                % value % id)
{
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_INVALID_PROPERTY_VALUE_H
