//-----------------------------------------------------------------------------
/** @file libboardgame_util/Barrier.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_BARRIER_H
#define LIBBOARDGAME_UTIL_BARRIER_H

#include <condition_variable>
#include <mutex>

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

/** Similar to boost::barrier, which does not exist in C++11 */
class Barrier
{
public:
    explicit Barrier(unsigned count);

    void wait();

private:
    mutex m_mutex;

    condition_variable m_condition;

    unsigned m_threshold;

    unsigned m_count;

    unsigned m_current = 0;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_BARRIER_H
