//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/Player.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Player.h"

#include <fstream>
#include <iomanip>
#include "libboardgame_base/CpuTimeSource.h"
#include "libboardgame_base/Memory.h"
#include "libboardgame_base/WallTimeSource.h"

namespace libpentobi_mcts {

using libboardgame_base::CpuTimeSource;
using libboardgame_base::WallTimeSource;
using libpentobi_base::BoardType;

//-----------------------------------------------------------------------------

namespace {

// Rationale for choosing the number of simulations:
// * Level 9, the highest in the desktop version, should be as strong as
//   possible on a mid-range PC with reasonable thinking times. The average
//   time per game and player is targeted at 2-3 min for the 2-color game
//   variants and 5-6 min for the others.
// * Level 7, the highest in the Android version, should be as strong as
//   possible on typical mobile hardware. It is set to 4% of level 9.
// * Level 8 is set to 20% of level 9, the middle (on a log scale) between
//   level 7 and 9. Since most parameter tuning is done at level 7 or 8, it is
//   better for development purposes to define level 8 in terms of time, even
//   if it doesn't necessarily correspond to the middle wrt. playing strength.
// * The numbers for level 1 are set to a value that is weak enough for
//   beginners without playing silly moves. They are currently chosen depending
//   on how strong we estimate Pentobi is in a game variant. It is also taken
//   into consideration how much the Elo difference level 1-9 is in self-play
//   experiments. After applying the scale factor (see comment in
//   Player::get_rating()), we want a range of about 1000 Elo (difference
//   between beginner and lower master level).
// * The numbers for level 1-6 are chosen such that they correspond to roughly
//   equidistant Elo differences measured in self-play experiments.
// * We only calibrate the numbers for the game variants we care most about.
//   For other game variants, we use the numbers of game variants with similar
//   playing strength and speed of simulations.

constexpr float counts_classic[Player::max_supported_level] =
    { 3, 30, 90, 181, 667, 5028, 69809, 349044, 1745221 };

constexpr float counts_duo[Player::max_supported_level] =
    { 3, 21, 77, 213, 861, 7280, 221867, 1109339, 5546695 };

constexpr float counts_trigon[Player::max_supported_level] =
    { 100, 246, 457, 876, 1882, 5506, 19819, 99092, 495465 };

constexpr float counts_nexos[Player::max_supported_level] =
    { 250, 347, 625, 1223, 3117, 8270, 20954, 104774, 523877 };

constexpr float counts_callisto_2[Player::max_supported_level] =
    { 30, 87, 300, 1017, 4729, 20435, 122778, 613905, 3069529 };

/** Suggest how much memory to use for the trees depending on the maximum
    level used. */
size_t get_memory(unsigned max_level)
{
    auto available = libboardgame_base::get_memory();
    if (available == 0)
    {
        LIBBOARDGAME_LOG("WARNING: could not determine system memory"
                         " (assuming 512MB)");
        available = 512000000;
    }
    // Don't use all of the available memory
    size_t reasonable = available / 4;
    size_t wanted = 2000000000;
    if (max_level < Player::max_supported_level)
    {
        // We don't need so much memory if m_max_level is smaller than
        // max_supported_level. Trigon has the highest relative number of
        // simulations on lower levels compared to the highest level. The
        // memory used in a search is not proportional to the number of
        // simulations (e.g. because the expand threshold increases with the
        // depth). We approximate this by adding an exponent to the ratio
        // and not taking into account if m_max_level is very small.
        static_assert(Player::max_supported_level >= 5);
        auto factor = pow(counts_trigon[Player::max_supported_level - 1]
                          / counts_trigon[max(max_level, 5u) - 1], 0.8);
        wanted = static_cast<size_t>(double(wanted) / factor);
    }
    size_t memory = min(wanted, reasonable);
    LIBBOARDGAME_LOG("Using ", memory / 1000000, " MB of ",
                     available / 1000000, " MB");
    return memory;
}

} // namespace

//-----------------------------------------------------------------------------

Player::Player(Variant initial_variant, unsigned max_level,
               const string&  books_dir, unsigned nu_threads)
    : m_is_book_loaded(false),
      m_use_book(true),
      m_resign(false),
      m_books_dir(books_dir),
      m_max_level(max_level),
      m_level(4),
      m_fixed_simulations(0),
      m_search(initial_variant, nu_threads, get_memory(max_level)),
      m_book(initial_variant),
      m_time_source(new WallTimeSource)
{
    for (unsigned i = 0; i < Board::max_player_moves; ++i)
    {
        // Hand-tuned such that time per move is more evenly spread among all
        // moves than with a fixed number of simulations (because the
        // simulations per second increase rapidly with the move number) but
        // the average time per game is roughly the same.
        m_weight_max_count_duo[i] = 0.7f * exp(0.1f * static_cast<float>(i));
        m_weight_max_count_classic[i] = m_weight_max_count_duo[i];
        m_weight_max_count_trigon[i] = m_weight_max_count_duo[i];
        m_weight_max_count_callisto[i] = m_weight_max_count_duo[i];
        m_weight_max_count_callisto_2[i] = m_weight_max_count_duo[i];
        // Less weight for the first move(s) because number of legal moves
        // is lower and the search applies some pruning rules to reduce the
        // branching factor in early moves
        if (i == 0)
        {
            m_weight_max_count_classic[i] *= 0.2f;
            m_weight_max_count_trigon[i] *= 0.2f;
            m_weight_max_count_duo[i] *= 0.6f;
            m_weight_max_count_callisto[i] *= 0.2f;
            m_weight_max_count_callisto_2[i] *= 0.2f;
        }
        else if (i == 1)
        {
            m_weight_max_count_classic[i] *= 0.2f;
            m_weight_max_count_trigon[i] *= 0.5f;
            m_weight_max_count_callisto[i] *= 0.6f;
            m_weight_max_count_callisto_2[i] *= 0.2f;
        }
        else if (i == 2)
        {
            m_weight_max_count_classic[i] *= 0.3f;
            m_weight_max_count_trigon[i] *= 0.6f;
        }
        else if (i == 3)
        {
            m_weight_max_count_trigon[i] *= 0.8f;
        }
    }
}

void Player::abort()
{
    m_search.abort();
    m_was_aborted = true;
}

Move Player::genmove(const Board& bd, Color c)
{
    m_resign = false;
    m_was_aborted = false;
    if (! bd.has_moves(c))
        return Move::null();
    Move mv;
    auto variant = bd.get_variant();
    auto board_type = bd.get_board_type();
    auto level = min(max(m_level, 1u), m_max_level);
    // Don't use more than 2 moves per color from opening book in lower levels
    if (m_use_book
        && (level >= 4 || bd.get_nu_moves() < 2u * bd.get_nu_colors()))
    {
        if (! is_book_loaded(variant))
            load_book(m_books_dir
                      + "/book_" + to_string_id(variant) + ".blksgf");
        if (m_is_book_loaded)
        {
            mv = m_book.genmove(bd, c);
            if (! mv.is_null())
                return mv;
        }
    }
    Float max_count = 0;
    double max_time = 0;
    if (m_fixed_simulations > 0)
        max_count = m_fixed_simulations;
    else if (m_fixed_time > 0)
        max_time = m_fixed_time;
    else
    {
        switch (board_type)
        {
        case BoardType::classic:
        case BoardType::gembloq_2:
            max_count = counts_classic[level - 1];
            break;
        case BoardType::duo:
            max_count = counts_duo[level - 1];
            break;
        case BoardType::trigon:
        case BoardType::trigon_3:
        case BoardType::callisto:
        case BoardType::callisto_3:
        case BoardType::gembloq:
        case BoardType::gembloq_3:
            max_count = counts_trigon[level - 1];
            break;
        case BoardType::nexos:
            max_count = counts_nexos[level - 1];
            break;
        case BoardType::callisto_2:
            max_count = counts_callisto_2[level - 1];
            break;
        }
        // Don't weight max_count in low levels, otherwise it is still too
        // strong for beginners (later in the game, the weight becomes much
        // greater than 1 because the simulations become very fast)
        bool weight_max_count = (level >= 4);
        if (weight_max_count)
        {
            auto player_move = bd.get_nu_onboard_pieces(c);
            float weight = 1; // Init to avoid compiler warning
            switch (board_type)
            {
            case BoardType::classic:
                weight = m_weight_max_count_classic[player_move];
                break;
            case BoardType::duo:
            case BoardType::gembloq_2:
                weight = m_weight_max_count_duo[player_move];
                break;
            case BoardType::callisto:
            case BoardType::callisto_3:
                weight = m_weight_max_count_callisto[player_move];
                break;
            case BoardType::callisto_2:
                weight = m_weight_max_count_callisto_2[player_move];
                break;
            case BoardType::trigon:
            case BoardType::trigon_3:
            case BoardType::nexos:
            case BoardType::gembloq:
            case BoardType::gembloq_3:
                weight = m_weight_max_count_trigon[player_move];
                break;
            }
            max_count = ceil(max_count * weight);
        }
    }
    if (max_count != 0)
        LIBBOARDGAME_LOG("MaxCnt ", fixed, setprecision(0), max_count);
    else
        LIBBOARDGAME_LOG("MaxTime ", max_time);
    if (! m_search.search(mv, bd, c, max_count, 0, max_time, *m_time_source))
        return Move::null();
    m_was_aborted = m_search.was_aborted();
    // Resign only in two-player game variants
    if (get_nu_players(variant) == 2)
        if (m_search.get_root_visit_count() > 500
                && m_search.get_root_val().get_mean() < 0.09f)
            m_resign = true;
    return mv;
}

Rating Player::get_rating(Variant variant, unsigned level)
{
    // The ratings are roughly based on Elo differences measured in self-play
    // experiments. The measured values are scaled with a factor smaller than 1
    // to take into account that self-play usually overestimates the strength
    // against humans. The anchor is set to about 1000 (beginner level) for
    // level 1. The exact value for anchor and scale is chosen according to our
    // estimate how strong Pentobi plays at level 1 and level 9 in each game
    // variant (2000 Elo would be lower expert level). Currently, only 2-player
    // variants are calibrated and the ratings are also used for other game
    // variants that we assume have comparable strength (e.g. multi-player on
    // the same board).
    level = min(max(level, 1u), max_supported_level);
    Rating result;
    switch (get_board_type(variant))
    {
    case BoardType::classic: // Measured for classic_2
        {
            // Anchor 1000, scale 0.6
            static double elo[max_supported_level] =
                { 1000, 1142, 1283, 1425, 1567, 1708, 1850, 1951, 2030 };
            result = Rating(elo[level - 1]);
        }
        break;
    case BoardType::duo:
        {
            // Anchor 1000, scale 0.74
            static double elo[max_supported_level] =
                { 1000, 1189, 1378, 1567, 1755, 1945, 2134, 2185, 2209 };
            result = Rating(elo[level - 1]);
        }
        break;
    case BoardType::callisto_2:
        {
            // Anchor 1000, scale 0.49
            static double elo[max_supported_level] =
                { 1000, 1113, 1225, 1338, 1450, 1563, 1675, 1783, 1868 };
            result = Rating(elo[level - 1]);
        }
        break;
    case BoardType::trigon: // Measured for trigon_2
    case BoardType::trigon_3:
        {
            // Anchor 1000, scale 0.48
            static double elo[max_supported_level] =
                { 1000, 1110, 1220, 1330, 1440, 1550, 1660, 1765, 1897 };
            result = Rating(elo[level - 1]);
        }
        break;
    case BoardType::nexos: // Measured for nexos_2
    case BoardType::callisto:
    case BoardType::callisto_3:
    case BoardType::gembloq:
    case BoardType::gembloq_2:
    case BoardType::gembloq_3:
        {
            // Anchor 1000, scale 0.60
            static double elo[Player::max_supported_level] =
                { 1000, 1101, 1202, 1304, 1406, 1507, 1608, 1698, 1799 };
            result = Rating(elo[level - 1]);
        }
        break;
    }
    return result;
}

bool Player::is_book_loaded(Variant variant) const
{
    return m_is_book_loaded && m_book.get_tree().get_variant() == variant;
}

void Player::load_book(istream& in)
{
    m_book.load(in);
    m_is_book_loaded = true;
}

bool Player::load_book(const string& filepath)
{
    ifstream in(filepath);
    if (! in)
    {
        LIBBOARDGAME_LOG("Could not load book ", filepath);
        return false;
    }
    m_book.load(in);
    m_is_book_loaded = true;
    LIBBOARDGAME_LOG("Loaded book ", filepath);
    return true;
}

bool Player::resign() const
{
    return m_resign;
}

void Player::use_cpu_time(bool enable)
{
    if (enable)
        m_time_source = make_unique<CpuTimeSource>();
    else
        m_time_source = make_unique<WallTimeSource>();
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
