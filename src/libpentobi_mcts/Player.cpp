//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/Player.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Player.h"

#include <boost/filesystem/fstream.hpp>
#include "libboardgame_util/WallTime.h"

namespace libpentobi_mcts {

using namespace std;
using boost::filesystem::ifstream;
using libboardgame_util::log;
using libboardgame_util::WallTime;
using libpentobi_base::game_variant_classic;
using libpentobi_base::game_variant_duo;
using libpentobi_base::GameVariant;

//-----------------------------------------------------------------------------

namespace {

const bool use_weight_max_count = true;

} // namespace

//-----------------------------------------------------------------------------

Player::Player(const Board& bd, const path& application_dir_path)
    : libpentobi_base::Player(bd),
      m_is_book_loaded(false),
      m_use_book(true),
      m_application_dir_path(application_dir_path),
      m_level(4),
      m_fixed_simulations(0),
      m_search(bd)
{
    for (unsigned int i = 0; i < Board::max_player_moves; ++i)
    {
        // Hand-tuned such that time per move is more evenly spread among all
        // moves than with a fixed number of simulations (because the
        // simulations per second increase rapidly with the move number) but
        // the average time per game is roughly the same
        weight_max_count_duo[i] = ValueType(0.7 * exp(0.1 * i));
        weight_max_count_classic[i] = weight_max_count_duo[i];
        // Less weight for the first move(s) because the branching factor is
        // lower there (especially in classic/classic_2)
        if (i == 0)
        {
            weight_max_count_classic[i] *= 0.2;
            weight_max_count_duo[i] *= 0.7;
        }
        else if (i == 1)
            weight_max_count_classic[i] *= 0.4;
    }
}

Player::~Player() throw()
{
}

Move Player::genmove(Color c)
{
    if (! m_bd.has_moves(c))
        return Move::pass();
    Move mv;
    GameVariant variant = m_bd.get_game_variant();
    // Don't use classic_2 book for now, contains bad I5 openings, needs to be
    // regenerated with the current version of Pentobi
    // if (m_use_book && variant != game_variant_classic)
    if (m_use_book && variant == game_variant_duo)
    {
        if (! m_is_book_loaded
            || m_book.get_tree().get_game_variant() != variant)
        {
            string filename;
            if (variant == game_variant_duo)
                filename = "book_duo.blksgf";
            else
                filename = "book_classic_2.blksgf";
            load_book(filename);
        }
        if (m_is_book_loaded)
        {
            double delta;
            if (m_level <= 1)
                delta = 0.05;
            else if (m_level <= 2)
                delta = 0.04;
            else if (m_level <= 3)
                delta = 0.03;
            else if (m_level <= 4)
                delta = 0.02;
            else if (m_level <= 5)
                delta = 0.015;
            else if (m_level >= 6)
                delta = 0.01;
            mv = m_book.genmove(m_bd, c, delta, 4 * delta);
            if (! mv.is_null())
                return mv;
        }
    }
    WallTime time_source;
    ValueType max_count = 0;
    double max_time = 0;
    if (m_fixed_simulations > 0)
        max_count = m_fixed_simulations;
    else if (m_fixed_time > 0)
        max_time = m_fixed_time;
    else
    {
        if (m_level <= 1)
            max_count = 125;
        else if (m_level >= 6)
            max_count = ValueType(125 * pow(2.0, (6 - 1) * 2));
        else
            max_count = ValueType(125 * pow(2.0, (m_level - 1) * 2));
        if (use_weight_max_count)
        {
            unsigned int player_move = m_bd.get_nu_moves() / Color::range;
            float weight;
            if (variant == game_variant_duo)
                weight = weight_max_count_duo[player_move];
            else
                weight = weight_max_count_classic[player_move];
            max_count = ceil(max_count * weight);
        }
    }
    if (max_count != 0)
        log() << "MaxCnt " << max_count << '\n';
    else
        log() << "MaxTime " << max_time << '\n';
    if (! m_search.search(mv, c, max_count, 0, max_time, time_source))
        return Move::null();
    return mv;
}

void Player::load_book(istream& in)
{
    m_book.load(in);
    m_is_book_loaded = true;
}

void Player::load_book(const string& filename)
{
    // Search the given file at the following locations (in this order):
    // 1. Directory of the main executable
    // 2. Subdirectory src/book relative to the source code directory
    // 3. Data installation directory used on Unix (DATADIR/pentobi)
    if (try_load_book(m_application_dir_path / filename))
        return;
#ifdef ABS_TOP_SRCDIR
    if (try_load_book(path(ABS_TOP_SRCDIR) / "src/book" / filename))
        return;
#endif
#ifdef DATADIR
    if (try_load_book(path(DATADIR) / "pentobi" / filename))
        return;
#endif
}

bool Player::try_load_book(const path& filepath)
{
    log() << "Trying to load " << filepath << "... ";
    if (! exists(filepath))
    {
        log() << "not found\n";
        return false;
    }
    ifstream in(filepath);
    m_book.load(in);
    m_is_book_loaded = true;
    log() << "ok\n";
    return true;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
