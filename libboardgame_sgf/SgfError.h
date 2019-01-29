//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/SgfError.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_SGF_ERROR_H
#define LIBBOARDGAME_SGF_SGF_ERROR_H

#include <sstream>
#include <stdexcept>

namespace libboardgame_sgf {

using namespace std;

//-----------------------------------------------------------------------------

/** Exception indicating a semantic error in the tree.
    This exception is used for semantic errors in SGF trees. If a SGF tree
    is loaded from an external file, it is usually only checked for
    (game-independent) syntax errors, but not for semantic errors (e.g. illegal
    moves) because that would be too expensive when loading large trees and
    not allow the user to partially use a tree if there is an error only in
    some variations. */
class SgfError
    : public runtime_error
{
    using runtime_error::runtime_error;
};

//-----------------------------------------------------------------------------

class MissingProperty
    : public SgfError
{
public:
    explicit MissingProperty(const string& id);
};

//-----------------------------------------------------------------------------

class InvalidProperty
    : public SgfError
{
public:
    template<typename T>
    InvalidProperty(const string& id, const T& value);

private:
    template<typename T>
    static string get_message(const string& id, const T& value);
};

template<typename T>
InvalidProperty::InvalidProperty(const string& id, const T& value)
    : SgfError(get_message(id, value))
{
}

template<typename T>
string InvalidProperty::get_message(const string& id, const T& value)
{
    ostringstream msg;
    msg << "Invalid value '" << value << "' for SGF property '" << id << "'";
    return msg.str();
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_SGF_ERROR_H
