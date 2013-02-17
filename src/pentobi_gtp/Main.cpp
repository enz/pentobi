//-----------------------------------------------------------------------------
/** @file pentobi_gtp/Main.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fstream>
#include <iostream>
#include <boost/format.hpp>
#include "Engine.h"
#include "libboardgame_util/Exception.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_util/Options.h"
#include "libboardgame_util/RandomGenerator.h"

using namespace std;
using boost::filesystem::path;
using boost::format;
using libboardgame_gtp::Failure;
using libboardgame_util::log;
using libboardgame_util::set_log_null;
using libboardgame_util::Exception;
using libboardgame_util::Options;
using libboardgame_util::RandomGenerator;
using libpentobi_base::parse_variant_id;
using libpentobi_base::Board;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

namespace {

path get_application_dir_path(int argc, char** argv)
{
    if (argc == 0 || argv == nullptr || argv[0] == nullptr)
        return path();
    path application_path(argv[0]);
    return application_path.branch_path();
}

} // namespace

//-----------------------------------------------------------------------------

int main(int argc, char** argv)
{
    path application_dir_path = get_application_dir_path(argc, argv);
    try
    {
        vector<string> specs{
            "book:", "config|c:", "color", "cputime", "game|g:", "help|h",
                "level|l:", "memory:", "nobook", "noresign", "quiet|q",
                "seed|r:", "showboard", "threads:", "version|v" };
        Options opt(argc, argv, specs);
        if (opt.contains("help"))
        {
            cout <<
                "Usage: pentobi_gtp [options] [input files]\n"
                "--book       load an external book file\n"
                "--config,-c  set GTP config file\n"
                "--color      colorize text output of boards\n"
                "--cputime    use CPU time\n"
                "--game,-g    game variant (classic, classic_2, duo, trigon,\n"
                "             trigon_2, junior)\n"
                "--help,-h    print help message and exit\n"
                "--level,-l   set playing strength level\n"
                "--memory     memory to allocate for search trees\n"
                "--seed,-r    set random seed\n"
                "--showboard  automatically write board to stderr after\n"
                "             changes\n"
                "--nobook     disable opening book\n"
                "--noresign   disable resign\n"
                "--quiet,-q   do not print logging messages\n"
                "--threads    number of threads in the search\n"
                "--version,-v print version and exit\n";
            return 0;
        }
        if (opt.contains("version"))
        {
#ifdef VERSION
            cout << "Pentobi " << VERSION << '\n';
#else
            cout << "Pentobi UNKNONW";
#endif
            return 0;
        }
        size_t memory = 0;
        if (opt.contains("memory"))
        {
            memory = opt.get<size_t>("memory");
            if (memory == 0)
                throw Exception("Value for memory must be greater zero.");
        }
        auto threads = opt.get<unsigned>("threads", 1);;
        if (threads == 0)
            throw Exception("Number of threads must be greater zero.");
        Board::color_output = opt.contains("color");
        if (opt.contains("quiet"))
            set_log_null();
        if (opt.contains("seed"))
            RandomGenerator::set_global_seed(opt.get<uint32_t>("seed"));
        string variant_string = opt.get("game", "classic");
        Variant variant;
        if (! parse_variant_id(variant_string, variant))
            throw Exception(format("invalid game variant '%1%'")
                            % variant_string);
        auto level = opt.get<int>("level", 4);
        auto use_book = (! opt.contains("nobook"));
        path books_dir = application_dir_path;
        pentobi_gtp::Engine engine(variant, level, use_book, books_dir,
                                   threads, memory);
        engine.set_resign(! opt.contains("noresign"));
        if (opt.contains("showboard"))
            engine.set_show_board(true);
        if (opt.contains("seed"))
            engine.set_deterministic();
        if (opt.contains("cputime"))
            engine.use_cpu_time(true);
        string book_file = opt.get("book", "");
        if (! book_file.empty())
        {
            ifstream in(book_file);
            engine.get_mcts_player().load_book(in);
        }
        string config_file = opt.get("config", "");
        if (! config_file.empty())
        {
            ifstream in(config_file);
            if (! in)
                throw Exception(format("Error opening '%1%'") % config_file);
            engine.exec(in, true, log());
        }
        auto& args = opt.get_args();
        if (! args.empty())
            for (auto& file : args)
            {
                ifstream in(file.c_str());
                if (! in)
                    throw Exception(format("Error opening '%1%'") % file);
                engine.exec_main_loop_st(in, cout);
            }
        else
            engine.exec_main_loop_st(cin, cout);
        return 0;
    }
    catch (const Failure& e)
    {
        log(format("Error: command in config file failed: %1%")
            % e.get_response());
        return 1;
    }
    catch (const exception& e)
    {
        log(format("Error: %1%") % e.what());
        return 1;
    }
    catch (const char* s)
    {
        log(format("Error: %1%") % s);
        return 1;
    }
    catch (...)
    {
        log("Error: unknown exception");
        return 1;
    }
}

//-----------------------------------------------------------------------------
