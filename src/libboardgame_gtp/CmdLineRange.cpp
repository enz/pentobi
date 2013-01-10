//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/CmdLineRange.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "CmdLineRange.h"

namespace libboardgame_gtp {

using namespace std;

//-----------------------------------------------------------------------------

CmdLineRange::operator string() const
{
    return string(m_range.begin(), m_range.end());
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp
