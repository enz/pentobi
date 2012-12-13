//-----------------------------------------------------------------------------
/** @file libboardgame_util/RandomGenerator.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_RANDOM_GENERATOR_H
#define LIBBOARDGAME_UTIL_RANDOM_GENERATOR_H

#include <cstdint>
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
#if defined __GNUC__ && ! defined __clang__ \
  && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 4))
    typedef mt19937 Generator;
#else
    /** This generator is the same as mt11213b in the Boost library.
        According to the documentation of Boost 1.52, this generator is a bit
        faster and uses less memory than mt19937. (The typedef mt11213b does
        not exist in C++11.) */
    typedef mersenne_twister_engine<uint32_t,32,351,175,19,0xccab8ee7,11,
                                    0xffffffff,7,0x31b6ab00,15,0xffe50000,17,
                                    1812433253> Generator;
#endif

    typedef Generator::result_type ResultType;

    /** Constructor.
        Constructs the random generator with the global seed, if one was
        defined, otherwise with a non-deterministic seed. */
    RandomGenerator();

    ~RandomGenerator() throw();

    void set_seed(ResultType seed);

    ResultType generate();

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
    Generator m_generator;

    uniform_real_distribution<float> m_float_distribution;
};

inline RandomGenerator::ResultType RandomGenerator::generate()
{
    return m_generator();
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

inline void RandomGenerator::set_seed(ResultType seed)
{
    m_generator.seed(seed);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_RANDOM_GENERATOR_H
