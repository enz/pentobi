//-----------------------------------------------------------------------------
/** @file twogtp/Main.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include <atomic>
#include <thread>
#include "Analyze.h"
#include "TwoGtp.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_util/Options.h"
#include "libpentobi_base/Variant.h"

using namespace std;
using libboardgame_util::Options;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

int main(int argc, char** argv)
{
    libboardgame_util::LogInitializer log_initializer;
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
            "quiet",
            "saveinterval:",
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
        auto black = opt.get("black");
        auto white = opt.get("white");
        auto prefix = opt.get("file", "output");
        auto nu_games = opt.get<unsigned>("nugames", 1);
        auto nu_threads = opt.get<unsigned>("threads", 1);
        auto variant_string = opt.get("game", "classic");
        auto save_interval = opt.get<double>("saveinterval", 60);
        bool quiet = opt.contains("quiet");
        if (quiet)
            libboardgame_util::disable_logging();
        bool fast_open = opt.contains("fastopen");
        bool create_tree = opt.contains("tree") || fast_open;
        Variant variant;
        if (! parse_variant_id(variant_string, variant))
            throw runtime_error("invalid game variant " + variant_string);
        Output output(variant, prefix, create_tree);
        vector<shared_ptr<TwoGtp>> twogtps;
        twogtps.reserve(nu_threads);
        for (unsigned i = 0; i < nu_threads; ++i)
        {
            string log_prefix;
            if (nu_threads > 1)
                log_prefix = to_string(i + 1);
            auto twogtp = make_shared<TwoGtp>(black, white, variant,
                                              nu_games, output, quiet,
                                              log_prefix, fast_open);
            twogtp->set_save_interval(save_interval);
            twogtps.push_back(twogtp);
        }
        vector<thread> threads;
        threads.reserve(nu_threads);
        for (auto& i : twogtps)
            threads.emplace_back([&i, &result]()
            {
                try
                {
                    i->run();
                }
                catch (const exception& e)
                {
                    LIBBOARDGAME_LOG("Error: ", e.what());
                    result = 1;
                }
            });
        for (auto& t : threads)
            t.join();
    }
    catch (const exception& e)
    {
        LIBBOARDGAME_LOG("Error: ", e.what());
        result = 1;
    }
    return result;
}

//-----------------------------------------------------------------------------
