//-----------------------------------------------------------------------------
/** @file libboardgame_base/Engine.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Engine.h"

#include "libboardgame_util/CpuTime.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_util/RandomGenerator.h"

namespace libboardgame_base {

using namespace std;
using libboardgame_gtp::Failure;
using libboardgame_util::flush_log;
using libboardgame_util::RandomGenerator;

//-----------------------------------------------------------------------------

Engine::Engine()
{
    add("cputime", &Engine::cmd_cputime);
    add("set_random_seed", &Engine::cmd_set_random_seed);
}

void Engine::cmd_cputime(Response& response)
{
    auto time = libboardgame_util::cpu_time();
    if (time < 0)
        throw Failure("cannot determine cpu time");
    response << time;
}

/** Set global random seed.
    Arguments: random seed */
void Engine::cmd_set_random_seed(Arguments args)
{
    RandomGenerator::set_global_seed(args.get<RandomGenerator::ResultType>());
}

void Engine::on_handle_cmd_begin()
{
    flush_log();
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
