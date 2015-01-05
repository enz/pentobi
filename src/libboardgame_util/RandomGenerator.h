//-----------------------------------------------------------------------------
/** @file libboardgame_util/RandomGenerator.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_RANDOM_GENERATOR_H
#define LIBBOARDGAME_UTIL_RANDOM_GENERATOR_H

#include <cstdint>
#include <random>
#include "Assert.h"

#ifndef LIBBOARDGAME_USE_SIMD_MERSENNE_TWISTER
// Use fast SIMD-based random generator if GCC. This is a non-standard
// extension of GCC >=4.8 and so far only available on little-endian systems
// (last checked with GCC 4.8.2). It also cannot be used on Android because
// ext/random tries to include ext/opt_random.h, which is missing in the
// Android NDK (last checked with android-ndk32-r10-linux-x86_64).
#if defined __GNUC__ \
    && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)) \
    && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ \
    && ! defined(ANDROID) && ! defined(__ANDROID__)
#define LIBBOARDGAME_USE_SIMD_MERSENNE_TWISTER 1
#endif
#endif

#if LIBBOARDGAME_USE_SIMD_MERSENNE_TWISTER
#include <ext/random>
#endif

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
    /** @typedef Generator
        This generator is the same as mt11213b in the Boost library.
        According to the documentation of Boost 1.52, this generator is a bit
        faster and uses less memory than mt19937. (The typedef mt11213b does
        not exist in C++11.) */
#if LIBBOARDGAME_USE_SIMD_MERSENNE_TWISTER
    typedef __gnu_cxx::sfmt11213 Generator;
#else
    typedef mersenne_twister_engine<uint32_t, 32, 351, 175, 19, 0xccab8ee7, 11,
                                    0xffffffff, 7, 0x31b6ab00, 15, 0xffe50000,
                                    17, 1812433253> Generator;
#endif

    typedef Generator::result_type ResultType;

    /** Constructor.
        Constructs the random generator with the global seed, if one was
        defined, otherwise with a non-deterministic seed. */
    RandomGenerator();

    ~RandomGenerator();

    void set_seed(ResultType seed);

    ResultType generate();

    /** Generate a floating point value in [0..1]. */
    double generate_double();

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

    uniform_real_distribution<double> m_double_distribution;
};

inline RandomGenerator::ResultType RandomGenerator::generate()
{
    return m_generator();
}

inline double RandomGenerator::generate_double()
{
    return m_double_distribution(m_generator);
}

inline void RandomGenerator::set_seed(ResultType seed)
{
    m_generator.seed(seed);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util

#endif // LIBBOARDGAME_UTIL_RANDOM_GENERATOR_H
