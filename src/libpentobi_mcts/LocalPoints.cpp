//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/LocalPoints.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "LocalPoints.h"

namespace libpentobi_mcts {

//-----------------------------------------------------------------------------

LocalPoints::LocalPoints()
{
    m_is_local.fill_all(false);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
