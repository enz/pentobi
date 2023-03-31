//-----------------------------------------------------------------------------
/** @file libboardgame_base/RandomGenerator.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_BASE_RANDOM_GENERATOR_H
#define LIBBOARDGAME_BASE_RANDOM_GENERATOR_H

#include <random>

namespace libboardgame_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Fast pseudo-random number generator.
    This is a fast and low-quality pseudo-random number generator for tasks
    like opening book move selection or even playouts in Monte-Carlo tree
    search (does not seem to be sensitive to the quality of the generator).
    All instances of this class register themselves automatically at a
    global list of random generators, such that the random seed can be
    changed at all existing generators with a single function call.
    Thread-safe after construction. */
class RandomGenerator
{
public:
    using Generator = minstd_rand;

    using ResultType = Generator::result_type;


    /** Set seed for all currently existing and future generators.
        If this function is never called, a non-deterministic seed is used. */
    static void set_global_seed(ResultType seed);

    /** Set seed to last seed for all currently existing and future
        generators.
        Sets the seed to the last seed that was set with set_seed(). If no seed
        was explicitly defined with set_seed(), then this function does
        nothing. */
    static void set_global_seed_last();

    /** Check if a global seed was set.
        User code might want to take more measures if a global seed was set to
        become fully deterministic (e.g. avoid decisions based on time
        measurements). */
    static bool has_global_seed();


    /** Constructor.
        Constructs the random generator with the global seed, if one was
        defined, otherwise with a non-deterministic seed. */
    RandomGenerator();

    ~RandomGenerator();

    RandomGenerator(const RandomGenerator&) = delete;
    RandomGenerator& operator=(const RandomGenerator&) = delete;

    void set_seed(ResultType seed) { m_generator.seed(seed); }

    ResultType generate() { return m_generator(); }

    /** Generate a float in [a..b]. */
    float generate_float(float a, float b);

private:
    Generator m_generator;
};

inline float RandomGenerator::generate_float(float a, float b)
{
    uniform_real_distribution<float> distribution(a, b);
    return distribution(m_generator);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base

#endif // LIBBOARDGAME_BASE_RANDOM_GENERATOR_H
