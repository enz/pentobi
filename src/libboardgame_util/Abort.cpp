//-----------------------------------------------------------------------------
/** @file Abort.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Abort.h"

namespace libboardgame_util {

//-----------------------------------------------------------------------------

namespace {

volatile bool abort = false;

} // namespace

//----------------------------------------------------------------------------

void clear_abort()
{
    abort = false;
}

bool get_abort()
{
    return abort;
}

void set_abort()
{
    abort = true;
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
