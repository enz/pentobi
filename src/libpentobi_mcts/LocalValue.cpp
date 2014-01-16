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

void LocalValue::clear()
{
    for (auto i = m_points.begin(); i != m_points.end(); ++i)
        m_point_value[*i] = 0;
    m_points.clear();
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
