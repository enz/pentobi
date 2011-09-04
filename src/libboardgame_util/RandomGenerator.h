//-----------------------------------------------------------------------------
/** @file RandomGenerator.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_RANDOM_GENERATOR_H
#define LIBBOARDGAME_UTIL_RANDOM_GENERATOR_H

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/lagged_fibonacci.hpp>
#include "Assert.h"

namespace libboardgame_util {

using boost::mt19937;
using boost::lagged_fibonacci607;

//-----------------------------------------------------------------------------

/** Fast pseudo-random number generator.
    All instances of this class register themselves automatically at a
    global list of random generators, such that the random seed can be
    changed at all existing generators with a single function call.
    (@ref libboardgame_doc_threadsafe_after_construction) */
class RandomGenerator
{
public:
    typedef mt19937::result_type ResultType;

    /** Constructor.
        Cinstructs the random generator with the global seed, if one was
        defined, otherwise with a non-deterministic seed. */
    RandomGenerator();

    ~RandomGenerator() throw();

    void set_seed(ResultType seed);

    /** Generate a small random number.
        Fast way to generate a small integer that avoids an expensive modulo
        operation. Uses only the lower 16 bits of the random generator.
        @param n The size of the range; must be smaller than 2^16
        @return A random number between 0 and n - 1 */
    int generate_small_int(int n);

    /** Like generate_small_int() but for unsigned int. */
    unsigned int generate_small_uint(unsigned int n);

    /** Generate a floating point value in [0..1]. */
    double generate_float();

    /** Set seed for all currently existing and future generators.
        If this function is never called, a non-deterministic seed is used. */
    static void set_global_seed(ResultType seed);

    /** Set seed to last seed for all currently existing and future
        generators.
        Sets the seed to the last seed that was set with set_seed(). If no seed
        was explicitely defined with set_seed(), then this function does
        nothing. */
    static void set_global_seed_last();

private:
    mt19937 m_generator;

    lagged_fibonacci607 m_float_generator;
};

inline void RandomGenerator::set_seed(ResultType seed)
{
    m_generator.seed(seed);
    m_float_generator.seed(seed);
}

inline double RandomGenerator::generate_float()
{
    return m_float_generator();
}

inline int RandomGenerator::generate_small_int(int n)
{
    LIBBOARDGAME_ASSERT(n < (1 << 16));
    int i = ((m_generator() & 0xffff) * n) >> 16;
    LIBBOARDGAME_ASSERT(i < n);
    return i;
}

inline unsigned int RandomGenerator::generate_small_uint(unsigned int n)
{
    LIBBOARDGAME_ASSERT(n < (1 << 16));
    unsigned int i = ((m_generator() & 0xffff) * n) >> 16;
    LIBBOARDGAME_ASSERT(i < n);
    return i;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_RANDOM_GENERATOR_H
