//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/Player.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_PLAYER_H
#define LIBPENTOBI_MCTS_PLAYER_H

#include "Search.h"
#include "libboardgame_base/Rating.h"
#include "libpentobi_base/Book.h"
#include "libpentobi_base/PlayerBase.h"

namespace libpentobi_mcts {

using libboardgame_base::Rating;
using libpentobi_base::Book;
using libpentobi_base::PlayerBase;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

class Player final
    : public PlayerBase
{
public:
    static constexpr unsigned max_supported_level = 9;

    /** Constructor.
        @param initial_variant Game variant to initialize the internal
        board with (may avoid unnecessary BoardConst creation for game variant
        that is never used)
        @param max_level The maximum level used
        @param books_dir Directory containing opening books.
        @param nu_threads The number of threads to use in the search (0 means
        to select a reasonable default value) */
    Player(Variant initial_variant, unsigned max_level, const string& books_dir,
           unsigned nu_threads = 0);

    Move genmove(const Board& bd, Color c) override;

    bool resign() const override;

    Float get_fixed_simulations() const;

    double get_fixed_time() const;

    /** Use a fixed number of simulations in the search.
        If set to a value greater than zero, this value will enforce a
        fixed number of simulations per search independent of the playing
        level. */
    void set_fixed_simulations(Float n);

    /** Use a fixed time limit per move.
        If set to a value greater than zero, this value will set a fixed
        (maximum) time per search independent of the playing level. */
    void set_fixed_time(double seconds);

    bool get_use_book() const;

    void set_use_book(bool enable);

    unsigned get_level() const;

    void set_level(unsigned level);

    /** Use CPU time instead of Wall time to measure time. */
    void use_cpu_time(bool enable);

    Search& get_search();

    void load_book(istream& in);

    /** Is a book loaded and compatible with a given game variant? */
    bool is_book_loaded(Variant variant) const;

    /** Get an estimated Elo-rating of a level.
        This rating is an estimated rating when playing vs. humans. Although
        it is based on computer vs. computer experiments, the ratings were
        modified and rescaled to take into account that self-play experiments
        usually overestimate the rating differences when playing against
        humans. */
    static Rating get_rating(Variant variant, unsigned level);

    /** Get an estimated Elo-rating of the current level. */
    Rating get_rating(Variant variant) const;

    /** Was last move generation based on an aborted search? */
    bool was_aborted() const { return m_was_aborted; }

private:
    bool m_is_book_loaded;

    bool m_use_book;

    bool m_resign;

    bool m_was_aborted;

    string m_books_dir;

    unsigned m_max_level;

    unsigned m_level;

    array<float, Board::max_player_moves> m_weight_max_count_classic;

    array<float, Board::max_player_moves> m_weight_max_count_trigon;

    array<float, Board::max_player_moves> m_weight_max_count_duo;

    array<float, Board::max_player_moves> m_weight_max_count_callisto;

    array<float, Board::max_player_moves> m_weight_max_count_callisto_2;

    Float m_fixed_simulations;

    double m_fixed_time;

    Search m_search;

    Book m_book;

    unique_ptr<TimeSource> m_time_source;


    void init_settings();

    bool load_book(const string& filepath);
};

inline Float Player::get_fixed_simulations() const
{
    return m_fixed_simulations;
}

inline double Player::get_fixed_time() const
{
    return m_fixed_time;
}

inline unsigned Player::get_level() const
{
    return m_level;
}

inline Rating Player::get_rating(Variant variant) const
{
    return get_rating(variant, m_level);
}

inline Search& Player::get_search()
{
    return m_search;
}

inline bool Player::get_use_book() const
{
    return m_use_book;
}

inline void Player::set_fixed_simulations(Float n)
{
    m_fixed_simulations = n;
    m_fixed_time = 0;
}

inline void Player::set_fixed_time(double seconds)
{
    m_fixed_time = seconds;
    m_fixed_simulations = 0;
}

inline void Player::set_level(unsigned level)
{
    m_level = level;
    m_fixed_simulations = 0;
    m_fixed_time = 0;
}

inline void Player::set_use_book(bool enable)
{
    m_use_book = enable;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_PLAYER_H
