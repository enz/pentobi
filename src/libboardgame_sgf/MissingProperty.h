//-----------------------------------------------------------------------------
/** @file libboardgame_sgf/MissingProperty.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_MISSING_PROPERTY_H
#define LIBBOARDGAME_SGF_MISSING_PROPERTY_H

#include "InvalidTree.h"

namespace libboardgame_sgf {

using namespace std;

//-----------------------------------------------------------------------------

class MissingProperty
    : public InvalidTree
{
public:
    MissingProperty(const string& message);

    MissingProperty(const string& id, const string& message);

    MissingProperty(const string& id, const format& message);
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_MISSING_PROPERTY_H
