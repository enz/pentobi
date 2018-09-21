//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/Atomic.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_ATOMIC_H
#define LIBBOARDGAME_MCTS_ATOMIC_H

#include <atomic>
#include "libboardgame_util/Unused.h"

namespace libboardgame_mcts {

using namespace std;

//-----------------------------------------------------------------------------

/** Data that may be atomic.
    This struct is used for sharing the same code for a single-threaded and
    a multi-threaded implementation depending on a template argument.
    In the multi-threaded implementation, the variable is atomic, which
    usually causes a small performance penalty, in the single-threaded
    implementation, it is simply a regular variable.
    @param T The type of the variable.
    @param MT true, if the variable should be atomic. */
template<typename T, bool MT> struct Atomic;

template<typename T>
struct Atomic<T, false>
{
    T val;

    Atomic& operator=(T t)
    {
        val = t;
        return *this;
    }

    T load(memory_order order = memory_order_seq_cst) const
    {
        LIBBOARDGAME_UNUSED(order);
        return val;
    }

    void store(T t, memory_order order = memory_order_seq_cst)
    {
        LIBBOARDGAME_UNUSED(order);
        val = t;
    }

    operator T() const
    {
        return val;
    }

    T fetch_add(T t)
    {
        T tmp = val;
        val += t;
        return tmp;
    }
};

template<typename T>
struct Atomic<T, true>
{
    atomic<T> val;

    Atomic& operator=(T t)
    {
        val.store(t);
        return *this;
    }

    T load(memory_order order = memory_order_seq_cst) const
    {
        return val.load(order);
    }

    void store(T t, memory_order order = memory_order_seq_cst)
    {
        val.store(t, order);
    }

    operator T() const
    {
        return load();
    }

    T fetch_add(T t)
    {
        return val.fetch_add(t);
    }
};

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_ATOMIC_H
