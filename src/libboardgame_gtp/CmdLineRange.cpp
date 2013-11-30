//-----------------------------------------------------------------------------
/** @file libboardgame_gtp/CmdLineRange.cpp
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "CmdLineRange.h"

#include <iostream>

namespace libboardgame_gtp {

//-----------------------------------------------------------------------------

CmdLineRange::operator string() const
{
    return string(m_begin, m_end);
}

void CmdLineRange::write(ostream& o) const
{
    o << string(*this);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_gtp
