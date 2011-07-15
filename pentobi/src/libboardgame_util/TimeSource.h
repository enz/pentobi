//-----------------------------------------------------------------------------
/** @file TimeSource.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_TIME_SOURCE_H
#define LIBBOARDGAME_UTIL_TIME_SOURCE_H

namespace libboardgame_util {

//-----------------------------------------------------------------------------

class TimeSource
{
public:
    virtual ~TimeSource() throw();

    /** Get the current time in seconds. */
    virtual double operator()() = 0;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_TIME_SOURCE_H
