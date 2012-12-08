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
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

Player::Player(Variant initial_variant, const path& books_dir, size_t memory)
    : m_is_book_loaded(false),
      m_use_book(true),
      m_resign(false),
      m_books_dir(books_dir),
      m_level(4),
      m_fixed_simulations(0),
      m_resign_threshold(0.09f),
      m_resign_min_simulations(500),
      m_search(initial_variant, memory),
      m_book(initial_variant)
{
    for (unsigned i = 0; i < Board::max_player_moves; ++i)
    {
        // Hand-tuned such that time per move is more evenly spread among all
        // moves than with a fixed number of simulations (because the
        // simulations per second increase rapidly with the move number) but
        // the average time per game is roughly the same.
        weight_max_count_duo[i] = Float(0.7 * exp(0.1 * i));
        weight_max_count_classic[i] = weight_max_count_duo[i];
        weight_max_count_trigon[i] = weight_max_count_duo[i];
        // Less weight for the first move(s) because number of legal moves
        // is lower and the search applies some pruning rules to reduce the
        // branching factor in early moves
        if (i == 0)
        {
            weight_max_count_classic[i] *= Float(0.2);
            weight_max_count_trigon[i] *= Float(0.2);
            weight_max_count_duo[i] *= Float(0.6);
        }
        else if (i == 1)
        {
            weight_max_count_classic[i] *= Float(0.2);
            weight_max_count_trigon[i] *= Float(0.5);
        }
        else if (i == 2)
        {
            weight_max_count_classic[i] *= Float(0.3);
            weight_max_count_trigon[i] *= Float(0.6);
        }
        else if (i == 3)
        {
            weight_max_count_trigon[i] *= Float(0.8);
        }
    }
}

Player::~Player() throw()
{
}

Move Player::genmove(const Board& bd, Color c)
{
    m_resign = false;
    if (! bd.has_moves(c))
        return Move::pass();
    Move mv;
    Variant variant = bd.get_variant();
    if (m_use_book && m_level >= 4)
    {
        if (! m_is_book_loaded
            || m_book.get_tree().get_variant() != variant)
        {
            string filename;
            if (variant == Variant::duo)
                filename = "book_duo.blksgf";
            else if (variant == Variant::junior)
                filename = "book_junior.blksgf";
            else if (variant == Variant::classic_2)
                filename = "book_classic_2.blksgf";
            else if (variant == Variant::classic)
                filename = "book_classic.blksgf";
            else if (variant == Variant::trigon_2)
                filename = "book_trigon_2.blksgf";
            else if (variant == Variant::trigon_3)
                filename = "book_trigon_3.blksgf";
            else
            {
                LIBBOARDGAME_ASSERT(variant == Variant::trigon);
                filename = "book_trigon.blksgf";
            }
            load_book(m_books_dir / filename);
        }
        if (m_is_book_loaded)
        {
            mv = m_book.genmove(bd, c);
            if (! mv.is_null())
                return mv;
        }
    }
    WallTime time_source;
    Float max_count = 0;
    double max_time = 0;
    if (m_fixed_simulations > 0)
        max_count = m_fixed_simulations;
    else if (m_fixed_time > 0)
        max_time = m_fixed_time;
    else
    {
        // The minimum number of simulations and increase factor per level are
        // chosen such that the total time per game and player at level 7 is
        // less than 20 min (10 min for Duo) even on somewhat outdated PC
        // hardware. (This also takes the additional weighting of the number of
        // simulations depending on the move number into account.)
        // The increase factor should be no less than 3-4 to produce a
        // noticable effect on playing strength between levels.
        // The minimum number of simulations is very small to avoid that level
        // 1 is too strong for absolute beginners. Note that using the search
        // with simulations much smaller than the branching factor works only
        // because node values are initialized with prior knowledge and the
        // final move selection based on the visit count uses the value as a
        // a tie-breaker.
        Float minimum;
        Float factor_per_level;
        if (variant == Variant::classic || variant == Variant::classic_2)
        {
            minimum = 5;
            factor_per_level = 5.92f;
        }
        else if (variant == Variant::trigon
                 || variant == Variant::trigon_2
                 || variant == Variant::trigon_3)
        {
            minimum = 10;
            factor_per_level = 4.31f;
        }
        else if (variant == Variant::junior)
        {
            minimum = 5;
            factor_per_level = 7.28f;
        }
        else
        {
            LIBBOARDGAME_ASSERT(variant == Variant::duo);
            minimum = 5;
            factor_per_level = 7.16f;
        }
        if (m_level <= 1)
            max_count = minimum;
        else
            max_count =
                Float(ceil(minimum * pow(factor_per_level, m_level - 1)));
        // Don't weight max_count in low levels, otherwise it is still too
        // strong for beginners (later in the game, the weight becomes much
        // greater than 1 because the simulations become very fast)
        bool weight_max_count = (m_level >= 4);
        if (weight_max_count)
        {
            unsigned player_move = bd.get_nu_onboard_pieces(c);
            float weight = 1;
            if (variant == Variant::duo || variant == Variant::junior)
                weight = weight_max_count_duo[player_move];
            else if (variant == Variant::classic
                     || variant == Variant::classic_2)
                weight = weight_max_count_classic[player_move];
            else if (variant == Variant::trigon
                     || variant == Variant::trigon_2
                     || variant == Variant::trigon_3)
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
    // Resign only in two-player game variants
    if (get_nu_players(variant) == 2)
    {
        const Search::Node& root = m_search.get_tree().get_root();
        if (root.get_count() > m_resign_min_simulations
            && root.get_value() < m_resign_threshold)
            m_resign = true;
    }
    return mv;
}

Rating Player::get_rating(Variant variant, int level)
{
    // The initial rating estimates for version 2.0 are loosely based on
    // earlier experiments that measured the rating differences depending on
    // different number of simulations in Pentobi 1.0. A general factor of 0.5
    // was applied to measured Elo differences to take into account that
    // self-play experiments usually overestimate Elo differences when
    // playing vs. humans. The ratings were anchored such that level 1 was
    // at beginner level (~1000 Elo) and level 6 at lower expert level (~2000
    // Elo), which corresponds to estimates of the performance of Pentobi 1.0
    // vs. humans. Not all game variants were tested (for example, the ratings
    // for Classic 2 was also used for Classic, and those of Duo for
    // Junior). Modifications for the estimated playing strength of Pentobi
    // 1.0 in different game variants were applied (e.g. stronger in Duo,
    // weaker in Trigon).
    // Ratings of future versions of Pentobi should be roughly calibrated by
    // testing vs. Pentobi 2.0 and the same factor of 0.5 should be used to
    // rescale Elo differences measured in self-play experiments such that
    // they are somewhat comparable. This avoids jumps in the ratings of humans
    // after an upgrade of Pentobi if the computer player is used to assign a
    // rating to the human user.
    level = max(level, 1);
    switch (variant)
    {
    case Variant::classic:
    case Variant::classic_2:
        {
            static float elo[] = { 1000, 1255, 1510, 1740, 1880, 1950, 1990 };
            if (level <= 7)
                return Rating(elo[level - 1]);
            else
                // Ratings for levels greater 7 are not really tested.
                return Rating(elo[6] + static_cast<float>(10 * (level - 7)));
        }
    case Variant::trigon:
    case Variant::trigon_2:
    case Variant::trigon_3:
        {
            static float elo[] = { 920, 1100, 1300, 1485, 1580, 1650, 1700 };
            if (level <= 7)
                return Rating(elo[level - 1]);
            else
                // Ratings for levels greater 7 are not really tested.
                return Rating(elo[6] + static_cast<float>(10 * (level - 7)));
        }
    case Variant::duo:
    case Variant::junior:
        {
            static float elo[] = { 1100, 1325, 1550, 1750, 1880, 1950, 2020 };
            if (level <= 7)
                return Rating(elo[level - 1]);
            else
                // Ratings for levels greater 7 are not really tested.
                return Rating(elo[6] + static_cast<float>(10 * (level - 7)));
        }
    }
    LIBBOARDGAME_ASSERT(false);
    return Rating(0);
}

void Player::load_book(istream& in)
{
    m_book.load(in);
    m_is_book_loaded = true;
}

bool Player::load_book(const path& filepath)
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

bool Player::resign() const
{
    return m_resign;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
