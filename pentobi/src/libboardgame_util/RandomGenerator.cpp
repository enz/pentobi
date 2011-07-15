//-----------------------------------------------------------------------------
/** @file RandomGenerator.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RandomGenerator.h"

#include <ctime>
#include <list>
#include <sstream>
#include <boost/foreach.hpp>

namespace libboardgame_util {

using namespace std;
using boost::mt11213b;

//----------------------------------------------------------------------------

namespace {

bool is_seed_set = false;

uint32_t the_seed;

list<RandomGenerator*>& get_all_generators()
{
    static list<RandomGenerator*> all_generators;
    return all_generators;
}

uint32_t get_nondet_seed()
{
    static mt11213b seed_generator;
    uint32_t seed = uint32_t(time(0));
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

void RandomGenerator::set_global_seed(uint32_t seed)
{
    is_seed_set = true;
    the_seed = seed;
    BOOST_FOREACH(RandomGenerator* i, get_all_generators())
        i->set_seed(the_seed);
}

void RandomGenerator::set_global_seed_last()
{
    if (is_seed_set)
    {
        BOOST_FOREACH(RandomGenerator* i, get_all_generators())
            i->set_seed(the_seed);
    }
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_util
