//-----------------------------------------------------------------------------
/** @file libboardgame_util/Barrier.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Barrier.h"

#include "Assert.h"

namespace libboardgame_util {

//----------------------------------------------------------------------------

Barrier::Barrier(unsigned count)
  : m_threshold(count),
    m_count(count)
{
    LIBBOARDGAME_ASSERT(count > 0);
}

void Barrier::wait()
{
    unique_lock<mutex> lock(m_mutex);
    unsigned current = m_current;
    if (--m_count == 0)
    {
        ++m_current;
        m_count = m_threshold;
        m_condition.notify_all();
    }
    else
        while (current == m_current)
            m_condition.wait(lock);
}

//----------------------------------------------------------------------------

} // namespace libboardgame_util
