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
using libpentobi_base::game_variant_classic_2;
using libpentobi_base::game_variant_duo;
using libpentobi_base::game_variant_trigon;
using libpentobi_base::game_variant_trigon_2;
using libpentobi_base::game_variant_trigon_3;
using libpentobi_base::GameVariant;

//-----------------------------------------------------------------------------

namespace {

const bool use_weight_max_count = true;

} // namespace

//-----------------------------------------------------------------------------

Player::Player(GameVariant initial_game_variant, const path& books_dir)
    : m_is_book_loaded(false),
      m_use_book(true),
      m_books_dir(books_dir),
      m_level(4),
      m_fixed_simulations(0),
      m_search(initial_game_variant),
      m_book(initial_game_variant)
{
    for (unsigned int i = 0; i < Board::max_player_moves; ++i)
    {
        // Hand-tuned such that time per move is more evenly spread among all
        // moves than with a fixed number of simulations (because the
        // simulations per second increase rapidly with the move number) but
        // the average time per game is roughly the same.
        weight_max_count_duo[i] = ValueType(0.7 * exp(0.1 * i));
        weight_max_count_classic[i] = weight_max_count_duo[i];
        weight_max_count_trigon[i] = weight_max_count_duo[i];
        // Less weight for the first move(s) because number of legal moves
        // is lower and the search applies some pruning rules to reduce the
        // branching factor in early moves
        if (i == 0)
        {
            weight_max_count_classic[i] *= ValueType(0.2);
            weight_max_count_trigon[i] *= ValueType(0.2);
            weight_max_count_duo[i] *= ValueType(0.6);
        }
        else if (i == 1)
        {
            weight_max_count_classic[i] *= ValueType(0.2);
            weight_max_count_trigon[i] *= ValueType(0.5);
        }
        else if (i == 2)
        {
            weight_max_count_classic[i] *= ValueType(0.3);
            weight_max_count_trigon[i] *= ValueType(0.6);
        }
        else if (i == 3)
        {
            weight_max_count_trigon[i] *= ValueType(0.8);
        }
    }
}

Player::~Player() throw()
{
}

Move Player::genmove(const Board& bd, Color c)
{
    if (! bd.has_moves(c))
        return Move::pass();
    Move mv;
    GameVariant variant = bd.get_game_variant();
    if (m_use_book)
    {
        if (! m_is_book_loaded
            || m_book.get_tree().get_game_variant() != variant)
        {
            string filename;
            if (variant == game_variant_duo)
                filename = "book_duo.blksgf";
            else if (variant == game_variant_classic_2)
                filename = "book_classic_2.blksgf";
            else if (variant == game_variant_classic)
                filename = "book_classic.blksgf";
            else if (variant == game_variant_trigon_2)
                filename = "book_trigon_2.blksgf";
            else if (variant == game_variant_trigon_3)
                filename = "book_trigon_3.blksgf";
            else
            {
                LIBBOARDGAME_ASSERT(variant == game_variant_trigon);
                filename = "book_trigon.blksgf";
            }
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
            mv = m_book.genmove(bd, c, delta, 2.5 * delta);
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
        // The minimum number of simulations and increase factor per level are
        // chosen such that the number of simulations is at minimum in the same
        // order of magnitude than the typical branching factor (or a bit less),
        // and that the total time per game and player at level 6 is less than
        // 20 min (10 min for Duo) even on somewhat outdated PC hardware. (This
        // also takes the additional weighting of the number of simulations
        // depending on the move number into account.)
        // The increase factor should be no less than 2 to produce a noticable
        // effect on playing strength between levels.
        ValueType minimum;
        ValueType factor_per_level;
        if (variant == game_variant_classic
            || variant == game_variant_classic_2)
        {
            minimum = 100;
            factor_per_level = 3.95f;
        }
        else if (variant == game_variant_trigon
                 || variant == game_variant_trigon_2
                 || variant == game_variant_trigon_3)
        {
            minimum = 170;
            factor_per_level = 2.7f;
        }
        else
        {
            LIBBOARDGAME_ASSERT(variant == game_variant_duo);
            minimum = 180;
            factor_per_level = 4.7f;
        }
        if (m_level <= 1)
            max_count = minimum;
        else
            max_count = ValueType(minimum * pow(factor_per_level, m_level - 1));
        if (use_weight_max_count)
        {
            unsigned int player_move = bd.get_nu_moves() / bd.get_nu_colors();
            float weight = 1;
            if (variant == game_variant_duo)
                weight = weight_max_count_duo[player_move];
            else if (variant == game_variant_classic
                     || variant == game_variant_classic_2)
                weight = weight_max_count_classic[player_move];
            else if (variant == game_variant_trigon
                     || variant == game_variant_trigon_2
                     || variant == game_variant_trigon_3)
                weight = weight_max_count_trigon[player_move];
            max_count = ceil(max_count * weight);
        }
    }
    if (max_count != 0)
        log() << "MaxCnt " << max_count << '\n';
    else
        log() << "MaxTime " << max_time << '\n';
    if (! m_search.search(mv, bd, c, max_count, 0, max_time, time_source))
        return Move::null();
    return mv;
}

/** Initialize settings that can be overridden with a file
    libpentobi_mcts.conf in the application directory. */
void Player::load_book(istream& in)
{
    m_book.load(in);
    m_is_book_loaded = true;
}

void Player::load_book(const string& filename)
{
    try_load_book(m_books_dir / filename);
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
