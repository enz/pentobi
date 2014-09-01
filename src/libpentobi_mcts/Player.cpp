//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/Player.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Player.h"

#include <fstream>
#include <iomanip>
#include "libboardgame_util/CpuTimeSource.h"
#include "libboardgame_util/WallTimeSource.h"

namespace libpentobi_mcts {

using namespace std;
using libboardgame_util::log;
using libboardgame_util::CpuTimeSource;
using libboardgame_util::WallTimeSource;
using libpentobi_base::BoardType;

//-----------------------------------------------------------------------------

Player::Player(Variant initial_variant, const string& books_dir,
               unsigned nu_threads, size_t memory)
    : m_is_book_loaded(false),
      m_use_book(true),
      m_resign(false),
      m_books_dir(books_dir),
      m_level(4),
      m_fixed_simulations(0),
      m_resign_threshold(0.09f),
      m_resign_min_simulations(500),
      m_search(initial_variant, nu_threads, memory),
      m_book(initial_variant),
      m_time_source(new WallTimeSource())
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
        // Less weight for the first move(s) because number of legal moves
        // is lower and the search applies some pruning rules to reduce the
        // branching factor in early moves
        if (i == 0)
        {
            m_weight_max_count_classic[i] *= 0.2f;
            m_weight_max_count_trigon[i] *= 0.2f;
            m_weight_max_count_duo[i] *= 0.6f;
        }
        else if (i == 1)
        {
            m_weight_max_count_classic[i] *= 0.2f;
            m_weight_max_count_trigon[i] *= 0.5f;
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

Player::~Player() throw()
{
}

Move Player::genmove(const Board& bd, Color c)
{
    m_resign = false;
    if (! bd.has_moves(c))
        return Move::pass();
    Move mv;
    auto variant = bd.get_variant();
    auto board_type = bd.get_board_type();
    int level = min(max(m_level, 1), 9);
    // Don't use more thane 2 moves per color from opening book in lower levels
    if (m_use_book
        && (level >= 4 || bd.get_nu_moves() < 2u * bd.get_nu_colors()))
    {
        if (! m_is_book_loaded
            || m_book.get_tree().get_variant() != variant)
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
        // Rationale for chosing max_count:
        // * The number at level 1 is very small in Classic/Duo to avoid that
        //   level 1 is too strong for absolute beginners (searches with such a
        //   small number of simulations still produce reasonable moves because
        //   of the prior initialization of node values.)
        // * The number at the highest level is chosen such that the average
        //   time per game and player is 2 min in Duo, 4 min in Classic, 5 min
        //   in Trigon on a Intel i3-4130. (This takes into account the
        //   weighting of max_count with the move number below.)
        // * The numbers for other levels are chosen such that they roughly
        //   correspond to equal Elo differences in self-play experiments.
        switch (variant)
        {
        case Variant::classic:
        case Variant::classic_2:
            {
                static float counts[] =
                    { 3, 24, 87, 213, 667, 1989, 10780, 98438, 1249009 };
                max_count = counts[level - 1];
            }
            break;
        case Variant::duo:
        case Variant::junior:
            {
                static float counts[] =
                    { 3, 17, 44, 123, 426, 1672, 6240, 51590, 5174795 };
                max_count = counts[level - 1];
            }
            break;
        case Variant::trigon:
        case Variant::trigon_2:
        case Variant::trigon_3:
            {
                static float counts[] =
                    { 228, 433, 727, 1501, 2912, 7395, 20828, 61138, 348842 };
                max_count = counts[level - 1];
            }
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
                weight = m_weight_max_count_duo[player_move];
                break;
            case BoardType::trigon:
            case BoardType::trigon_3:
                weight = m_weight_max_count_trigon[player_move];
                break;
            }
            max_count = ceil(max_count * weight);
        }
    }
    if (max_count != 0)
        log("MaxCnt ", fixed, setprecision(0), max_count);
    else
        log("MaxTime ", max_time);
    if (! m_search.search(mv, bd, c, max_count, 0, max_time, *m_time_source))
        return Move::null();
    // Resign only in two-player game variants
    if (get_nu_players(variant) == 2)
    {
        auto& root = m_search.get_tree().get_root();
        if (root.get_visit_count() > m_resign_min_simulations
            && root.get_value() < m_resign_threshold)
            m_resign = true;
    }
    return mv;
}

Rating Player::get_rating(Variant variant, int level)
{
    // The ratings are based on experiments that measured the winning rates in
    // games between subsequent playing levels of Pentobi. The number of
    // simulations per level is already chosen such that the levels have
    // roughly equal Elo differences, see Player::genmove(). A scale factor
    // less than 1 was applied to the Elo differences to take into account that
    // self-play experiments usually overestimate the performance against
    // humans. The scale factor and lower anchor for the Elo ratings were
    // chosen such that the Elo range is roughly between 1000 (beginner level)
    // and 2000 (lower expert level) taking into account that Pentobi is
    // relatively strong against humans in Duo and relatively weak in Trigon.
    // Only the most popular game variants were tested (Classic 2, Duo,
    // Trigon 2) and these ratings were used for other game variants on the
    // same board type. The ratings are not always measured again for new
    // versions of Pentobi and not really comparable between different versions
    // of Pentobi.
    level = min(max(level, 1), 9);
    switch (variant)
    {
    case Variant::classic:
    case Variant::classic_2:
        {
            // Anchor 1000, scale 0.63
            static float elo[] =
                { 1000, 1134, 1267, 1400, 1534, 1668, 1801, 1935, 2068 };
            return Rating(elo[level - 1]);
        }
    case Variant::duo:
    case Variant::junior:
        {
            // Anchor 1100, scale 0.80
            static float elo[] =
                { 1100, 1229, 1359, 1489, 1618, 1748, 1878, 2008, 2137 };
            return Rating(elo[level - 1]);
        }
    case Variant::trigon:
    case Variant::trigon_2:
    case Variant::trigon_3:
        {
            // Anchor 1000, scale 0.60
            static float elo[] =
                { 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800 };
            return Rating(elo[level - 1]);
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

bool Player::load_book(const string& filepath)
{
    log("Trying to load ", filepath, "... ");
    ifstream in(filepath);
    if (! in)
    {
        log("not found");
        return false;
    }
    m_book.load(in);
    m_is_book_loaded = true;
    log("ok");
    return true;
}

bool Player::resign() const
{
    return m_resign;
}

void Player::use_cpu_time(bool enable)
{
    if (enable)
        m_time_source.reset(new CpuTimeSource());
    else
        m_time_source.reset(new WallTimeSource());
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts
