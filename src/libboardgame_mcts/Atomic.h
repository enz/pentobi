//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/Atomic.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_ATOMIC_H
#define LIBBOARDGAME_MCTS_ATOMIC_H

//-----------------------------------------------------------------------------

// If no support for multi-threaded search is needed, defining
// LIBBOARDGAME_MCTS_SINGLE_THREAD will use normal instead of atomic variables,
// which can speed up the search by a small amount.
#if LIBBOARDGAME_MCTS_SINGLE_THREAD


namespace libboardgame_mcts {

template<typename T, typename V>
inline T fetch_add_single_thread(T& x, const V& val)
{
    auto tmp = x;
    x += val;
    return tmp;
}

} // namespace libboardgame_mcts

#define LIBBOARDGAME_MCTS_ATOMIC(X) X

#define LIBBOARDGAME_MCTS_ATOMIC_LOAD(x, mem_order) x

#define LIBBOARDGAME_MCTS_ATOMIC_STORE(x, v, mem_order) x = v

#define LIBBOARDGAME_MCTS_ATOMIC_FETCH_ADD(x, v) fetch_add_single_thread(x, v)


#else // LIBBOARDGAME_MCTS_SINGLE_THREAD


#include <atomic>

#define LIBBOARDGAME_MCTS_ATOMIC(X) std::atomic<X>

#define LIBBOARDGAME_MCTS_ATOMIC_LOAD(x, mem_order) x.load(mem_order)

#define LIBBOARDGAME_MCTS_ATOMIC_STORE(x, v, mem_order) x.store(v, mem_order)

#define LIBBOARDGAME_MCTS_ATOMIC_FETCH_ADD(x, v) x.fetch_add(v)


#endif // LIBBOARDGAME_MCTS_SINGLE_THREAD

//-----------------------------------------------------------------------------

#endif // LIBBOARDGAME_MCTS_ATOMIC_H
