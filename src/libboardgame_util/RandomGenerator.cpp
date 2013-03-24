//-----------------------------------------------------------------------------
/** @file libboardgame_util/RandomGenerator.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RandomGenerator.h"

#include <ctime>
#include <list>

namespace libboardgame_util {

using namespace std;

//----------------------------------------------------------------------------

namespace {

bool is_seed_set = false;

RandomGenerator::ResultType the_seed;

list<RandomGenerator*>& get_all_generators()
{
    static list<RandomGenerator*> all_generators;
    return all_generators;
}

RandomGenerator::ResultType get_nondet_seed()
{
    // std::random_device should be used instead of std::time() in the future
    // but random_device is still broken on MinGW GCC 4.7.2 (it always throws)
    static RandomGenerator::Generator seed_generator;
    auto seed = static_cast<RandomGenerator::ResultType>(time(nullptr));
    seed ^= seed_generator();
    return seed;
}

} // namespace

//-----------------------------------------------------------------------------

RandomGenerator::RandomGenerator()
{
    set_seed(is_seed_set ? the_seed : get_nondet_seed());
    get_all_generators().push_back(this);
}

RandomGenerator::~RandomGenerator() throw()
{
    get_all_generators().remove(this);
}

void RandomGenerator::set_global_seed(ResultType seed)
{
    is_seed_set = true;
    the_seed = seed;
    for (RandomGenerator* i : get_all_generators())
        i->set_seed(the_seed);
}

void RandomGenerator::set_global_seed_last()
{
    if (is_seed_set)
        for (RandomGenerator* i : get_all_generators())
            i->set_seed(the_seed);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util
