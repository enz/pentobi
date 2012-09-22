//-----------------------------------------------------------------------------
/** @file libboardgame_util/RandomGenerator.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_RANDOM_GENERATOR_H
#define LIBBOARDGAME_UTIL_RANDOM_GENERATOR_H

#include <random>
#include "Assert.h"

namespace libboardgame_util {

using namespace std;

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

    /** Like generate_small_int() but for unsigned. */
    unsigned generate_small_uint(unsigned n);

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

    // GCC 4.4 does not use the name uniform_real_distribution (as in C++11)
    // but uniform_real. This workaround can be removed when we begin using
    // other C++11 features that require GCC >4.4 anyway
#if defined __GNUC__ && ! defined __clang__ \
  && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 4))
    uniform_real<float> m_float_distribution;
#else
    uniform_real_distribution<float> m_float_distribution;
#endif
};

inline void RandomGenerator::set_seed(ResultType seed)
{
    m_generator.seed(seed);
}

inline double RandomGenerator::generate_float()
{
    return m_float_distribution(m_generator);
}

inline int RandomGenerator::generate_small_int(int n)
{
    return static_cast<int>(generate_small_uint(static_cast<unsigned>(n)));
}

inline unsigned RandomGenerator::generate_small_uint(unsigned n)
{
    LIBBOARDGAME_ASSERT(n < (1 << 16));
    unsigned i = static_cast<unsigned>(((m_generator() & 0xffff) * n) >> 16);
    LIBBOARDGAME_ASSERT(i < n);
    return i;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_RANDOM_GENERATOR_H
