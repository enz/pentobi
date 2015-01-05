//-----------------------------------------------------------------------------
/** @file libboardgame_util/RandomGenerator.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_UTIL_RANDOM_GENERATOR_H
#define LIBBOARDGAME_UTIL_RANDOM_GENERATOR_H

#include <random>

namespace libboardgame_util {

using namespace std;

//-----------------------------------------------------------------------------

/** Fast pseudo-random number generator.
    This is a fast and low-quality pseudo-random number generator for tasks
    like opening book move selection or even playouts in Monte-Carlo tree
    search (does not seem to be sensitive to the quality of the generator).
    All instances of this class register themselves automatically at a
    global list of random generators, such that the random seed can be
    changed at all existing generators with a single function call.
    (@ref libboardgame_doc_threadsafe_after_construction) */
class RandomGenerator
{
public:
    typedef minstd_rand Generator;

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
