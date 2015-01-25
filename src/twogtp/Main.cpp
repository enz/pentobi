//-----------------------------------------------------------------------------
/** @file twogtp/Main.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <atomic>
#include <thread>
#include "Analyze.h"
#include "TwoGtp.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_util/Options.h"
#include "libpentobi_base/Variant.h"

using namespace std;
using libboardgame_util::log;
using libboardgame_util::to_string;
using libboardgame_util::Options;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

int main(int argc, char** argv)
{
    atomic<int> result(0);
    try
    {
        vector<string> specs = {
            "analyze:",
            "black|b:",
            "fastopen",
            "file|f:",
            "game|g:",
            "nugames|n:",
            "splitsgf:",
            "threads:",
            "tree",
            "white|w:",
        };
        Options opt(argc, argv, specs);
        if (opt.contains("analyze"))
        {
            analyze(opt.get("analyze"));
            return 0;
        }
        if (opt.contains("splitsgf"))
        {
            splitsgf(opt.get("splitsgf"));
            return 0;
        }
        auto black = opt.get("black");
        auto white = opt.get("white");
        auto prefix = opt.get("file", "output");
        auto nu_games = opt.get<unsigned>("nugames", 1);
        auto nu_threads = opt.get<unsigned>("threads", 1);
        auto variant_string = opt.get("game", "classic");
        bool fast_open = opt.contains("fastopen");
        bool create_tree = opt.contains("tree") || fast_open;
        Variant variant;
        if (! parse_variant_id(variant_string, variant))
            throw Exception("invalid game variant " + variant_string);
        Output output(variant, prefix, create_tree);
        vector<shared_ptr<TwoGtp>> twogtp;
        for (unsigned i = 0; i < nu_threads; ++i)
        {
            string log_prefix;
            if (nu_threads > 1)
                log_prefix = to_string(i + 1);
            twogtp.push_back(make_shared<TwoGtp>(black, white, variant,
                                                 nu_games, output, false,
                                                 log_prefix, fast_open));
        }
        vector<thread> threads;
        for (auto& i : twogtp)
            threads.push_back(thread([&i, &result]()
            {
                try
                {
                    i->run();
                }
                catch (const exception& e)
                {
                    log("Error: ", e.what());
                    result = 1;
                }
            }));
        for (auto& t : threads)
            t.join();
    }
    catch (const exception& e)
    {
        log("Error: ", e.what());
        result = 1;
    }
    return result;
}

//-----------------------------------------------------------------------------
