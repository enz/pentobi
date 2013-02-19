//-----------------------------------------------------------------------------
/** @file libboardgame_util/Barrier.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_BARRIER_H
#define LIBBOARDGAME_UTIL_BARRIER_H

#include "Assert.h"

#ifdef USE_BOOST_THREAD
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#else
#include <condition_variable>
#include <mutex>
#endif

namespace libboardgame_util {

using namespace std;

#ifdef USE_BOOST_THREAD
using boost::condition_variable;
using boost::mutex;
#endif

//-----------------------------------------------------------------------------

/** Similar to boost::barrier, which does not exist in C++11 */
class Barrier
{
public:
    Barrier(unsigned count);

    void wait();

private:
    mutex m_mutex;

    condition_variable m_condition;

    unsigned m_threshold;

    unsigned m_count;

    unsigned m_current;
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_BARRIER_H
