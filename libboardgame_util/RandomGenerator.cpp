//-----------------------------------------------------------------------------
/** @file libboardgame_util/RandomGenerator.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "RandomGenerator.h"

#include <list>

namespace libboardgame_util {

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
    random_device generator;
    return generator();
}

} // namespace

//-----------------------------------------------------------------------------

RandomGenerator::RandomGenerator()
{
    set_seed(is_seed_set ? the_seed : get_nondet_seed());
    get_all_generators().push_back(this);
}

RandomGenerator::~RandomGenerator()
{
    get_all_generators().remove(this);
}

bool RandomGenerator::has_global_seed()
{
    return is_seed_set;
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
