//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/InvalidPropertyValue.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_INVALID_PROPERTY_VALUE_H
#define LIBBOARDGAME_SGF_INVALID_PROPERTY_VALUE_H

#include "InvalidTree.h"

#include "sstream"

namespace libboardgame_sgf {

using namespace std;

//-----------------------------------------------------------------------------

class InvalidPropertyValue
    : public InvalidTree
{
public:
    template<typename T>
    InvalidPropertyValue(const string& id, const T& value);

    ~InvalidPropertyValue() throw();

private:
    template<typename T>
    static string get_message(const string& id, const T& value);
};

template<typename T>
InvalidPropertyValue::InvalidPropertyValue(const string& id, const T& value)
    : InvalidTree(get_message(id, value))
{
}

template<typename T>
string InvalidPropertyValue::get_message(const string& id, const T& value)
{
    ostringstream msg;
    msg << "Invalid value '" << value << " for SGF property '" << id << "'";
    return msg.str();
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_INVALID_PROPERTY_VALUE_H
