//-----------------------------------------------------------------------------
/** @file MissingProperty.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_SGF_MISSING_PROPERTY_H
#define LIBBOARDGAME_SGF_MISSING_PROPERTY_H

#include "libboardgame_util/Exception.h"

namespace libboardgame_sgf {

using namespace std;
using boost::format;
using libboardgame_util::Exception;

//-----------------------------------------------------------------------------

class MissingProperty
    : public Exception
{
public:
    MissingProperty(const string& message);

    MissingProperty(const string& id, const string& message);

    MissingProperty(const string& id, const format& message);
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_sgf

#endif // LIBBOARDGAME_SGF_MISSING_PROPERTY_H
