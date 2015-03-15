//-----------------------------------------------------------------------------
/** @file libboardgame_base/Engine.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Engine.h"

#include "libboardgame_sys/CpuTime.h"
#include "libboardgame_util/Abort.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_util/RandomGenerator.h"

namespace libboardgame_base {

using namespace std;
using libboardgame_gtp::Failure;
using libboardgame_util::clear_abort;
using libboardgame_util::get_log;
using libboardgame_util::set_abort;
using libboardgame_util::RandomGenerator;

//-----------------------------------------------------------------------------

Engine::Engine()
{
    add("cputime", &Engine::cmd_cputime);
    add("cputime_diff", &Engine::cmd_cputime_diff);
    add("set_random_seed", &Engine::cmd_set_random_seed);
}

Engine::~Engine()
{
}

void Engine::cmd_cputime(Response& response)
{
    double time = libboardgame_sys::cpu_time();
    if (time == -1)
        throw Failure("cannot determine cpu time");
    response << time;
}

void Engine::cmd_cputime_diff(Response& response)
{
    double time = libboardgame_sys::cpu_time();
    if (time == -1)
        throw Failure("cannot determine cpu time");
    double diff;
    if (m_cpu_time_last == -1)
        diff = time;
    else
        diff = time - m_cpu_time_last;
    m_cpu_time_last = time;
    response << diff;
}

/** Set global random seed.
    Compatible with @ref libboardgame_doc_gnugo <br>
    Arguments: random seed */
void Engine::cmd_set_random_seed(const Arguments& args)
{
    RandomGenerator::set_global_seed(args.parse<RandomGenerator::ResultType>());
}

void Engine::on_handle_cmd_begin()
{
    clear_abort();
    get_log() << flush;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_base
