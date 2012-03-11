//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/InvalidPropertyValue.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_INVALID_PROPERTY_VALUE_H
#define LIBBOARDGAME_SGF_INVALID_PROPERTY_VALUE_H

#include "InvalidTree.h"

namespace libboardgame_sgf {

using namespace std;
using boost::format;

//-----------------------------------------------------------------------------

class InvalidPropertyValue
    : public InvalidTree
{
public:
    template<typename T>
    InvalidPropertyValue(const string& id, const T& value);

    ~InvalidPropertyValue() throw();
};

template<typename T>
InvalidPropertyValue::InvalidPropertyValue(const string& id, const T& value)
    : InvalidTree(format("Invalid value '%1%' for SGF property '%2%'")
                  % value % id)
{
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_INVALID_PROPERTY_VALUE_H
