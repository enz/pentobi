//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/LocalValue.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "LocalValue.h"

namespace libpentobi_mcts {

//-----------------------------------------------------------------------------

LocalValue::LocalValue()
{
    m_point_value.fill_all(0);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
