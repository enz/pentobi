//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/Player.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_PLAYER_H
#define LIBPENTOBI_MCTS_PLAYER_H

#include "Search.h"
#include "libboardgame_base/Rating.h"
#include "libboardgame_base/WallTimeSource.h"
#include "libpentobi_base/Book.h"
#include "libpentobi_base/PlayerBase.h"

namespace libpentobi_mcts {

using libboardgame_base::Rating;
using libboardgame_base::WallTimeSource;
using libpentobi_base::Book;
using libpentobi_base::PlayerBase;

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

    bool get_use_book() const;

    void set_use_book(bool enable);

    void set_level(unsigned level);

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

    /** Abort the current search.
        This function is intended to be called from a different thread to stop
        a currently running genmove(). The move generation will still return
        the best move found so far. Any calls to was_aborted() will return
        true until the next genmove() is started. */
    void abort();

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

    Search m_search;

    Book m_book;

    WallTimeSource m_time_source;


    void init_settings();

    bool load_book(const string& filepath);
};

inline Search& Player::get_search()
{
    return m_search;
}

inline bool Player::get_use_book() const
{
    return m_use_book;
}

inline void Player::set_level(unsigned level)
{
    m_level = level;
}

inline void Player::set_use_book(bool enable)
{
    m_use_book = enable;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_PLAYER_H
