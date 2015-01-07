//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/PlayoutFeatures.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PlayoutFeatures.h"

namespace libpentobi_mcts {

//-----------------------------------------------------------------------------

PlayoutFeatures::PlayoutFeatures()
{
    m_point_value.fill_all(0);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
