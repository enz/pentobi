//-----------------------------------------------------------------------------
/** @file pentobi_gtp/Main.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fstream>
#include <iostream>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include "Engine.h"
#include "libboardgame_util/Exception.h"
#include "libboardgame_util/Log.h"
#include "libboardgame_util/RandomGenerator.h"
#include "libpentobi_mcts/BookBuilder.h"

using namespace std;
using boost::filesystem::path;
using boost::format;
using boost::program_options::command_line_parser;
using boost::program_options::options_description;
using boost::program_options::notify;
using boost::program_options::positional_options_description;
using boost::program_options::store;
using boost::program_options::value;
using boost::program_options::variables_map;
using libboardgame_gtp::Failure;
using libboardgame_util::log;
using libboardgame_util::set_log_null;
using libboardgame_util::Exception;
using libboardgame_util::RandomGenerator;
using libpentobi_base::game_variant_classic;
using libpentobi_base::game_variant_classic_2;
using libpentobi_base::game_variant_duo;
using libpentobi_base::game_variant_trigon;
using libpentobi_base::game_variant_trigon_2;
using libpentobi_base::Board;
using libpentobi_base::GameVariant;
using libpentobi_mcts::BookBuilder;

//-----------------------------------------------------------------------------

namespace {

path get_application_dir_path(int argc, char** argv)
{
    if (argc == 0 || argv == 0 || argv[0] == 0)
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
        uint32_t seed;
        string config_file;
        string book_file;
        string build_book_file;
        string game_variant_string;
        vector<string> input_file;
        options_description normal_options("Options");
        int level = 4;
        double prune_book_diff;
        normal_options.add_options()
            ("book", value<>(&book_file), "load an external book file")
            ("buildbook", value<>(&build_book_file),
             "run the opening book builder")
            ("config,c", value<>(&config_file), "set GTP config file")
            ("color", "colorize text output of boards")
            ("game,g", value<>(&game_variant_string),
             "game variant (classic, classic_2, duo, trigon, trigon_2)")
            ("help,h", "print help message and exit")
            ("level,l", value<int>(&level), "set playing strength level")
            ("seed,r", value<uint32_t>(&seed), "set random seed")
            ("showboard", "automatically write board to stderr after changes")
            ("nobook", "do not use opening book")
            ("prunebook", value<double>(&prune_book_diff),
             "prune opening book (requires --buildbook)")
            ("quiet,q", "do not print logging messages")
            ("version,v", "print version and exit");
        options_description hidden_options;
        hidden_options.add_options()
            ("input-file", value<vector<string>>(&input_file),
             "input file");
        options_description all_options;
        all_options.add(normal_options).add(hidden_options);
        positional_options_description positional_options;
        positional_options.add("input-file", -1);
        variables_map vm;
        store(command_line_parser(argc, argv).options(all_options).
              positional(positional_options).run(), vm);
        notify(vm);
        if (vm.count("help"))
        {
            cout << "Usage: pentobi_gtp [options] [input files]\n"
                 << normal_options;
            return EXIT_SUCCESS;
        }
        if (vm.count("version"))
        {
#ifdef VERSION
            cout << "Pentobi " << VERSION << '\n';
#else
            cout << "Pentobi UNKNONW";
#endif
            return EXIT_SUCCESS;
        }
        Board::color_output = (vm.count("color") != 0);
        if (vm.count("quiet"))
            set_log_null();
        if (vm.count("seed"))
            RandomGenerator::set_global_seed(seed);
        GameVariant game_variant;
        if (game_variant_string == "duo" || game_variant_string == "")
            game_variant = game_variant_duo;
        else if (game_variant_string == "classic")
            game_variant = game_variant_classic;
        else if (game_variant_string == "classic_2"
                 || game_variant_string == "c2")
            game_variant = game_variant_classic_2;
        else if (game_variant_string == "trigon")
            game_variant = game_variant_trigon;
        else if (game_variant_string == "trigon_2")
            game_variant = game_variant_trigon_2;
        else
            throw Exception(format("invalid game variant '%1%'")
                            % game_variant_string);
        if (vm.count("buildbook"))
        {
            BookBuilder book_builder(game_variant);
            if (vm.count("prunebook"))
                book_builder.prune(build_book_file, prune_book_diff);
            else
                book_builder.build(build_book_file);
            return EXIT_SUCCESS;
        }
        bool use_book = (vm.count("nobook") == 0);
        pentobi_gtp::Engine engine(game_variant, level, use_book,
                                   application_dir_path);
        if (vm.count("showboard"))
            engine.set_show_board(true);
        if (vm.count("book"))
        {
            ifstream in(book_file);
            engine.get_mcts_player().load_book(in);
        }
        if (! config_file.empty())
        {
            ifstream in(config_file.c_str());
            if (! in)
                throw Exception(format("Error opening '%1%'") % config_file);
            engine.exec(in, true, log());
        }
        if (! input_file.empty())
        {
            BOOST_FOREACH(string file, input_file)
            {
                ifstream in(file.c_str());
                if (! in)
                    throw Exception(format("Error opening '%1%'") % file);
                engine.exec_main_loop_st(in, cout);
            }
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
