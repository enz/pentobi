//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/Player.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_PLAYER_H
#define LIBPENTOBI_MCTS_PLAYER_H

#include "Search.h"
#include "libpentobi_base/Book.h"
#include "libpentobi_base/Player.h"

namespace libpentobi_mcts {

using libpentobi_base::Book;

//-----------------------------------------------------------------------------

class Player
    : public libpentobi_base::Player
{
public:
    Player(const Board& bd);

    ~Player() throw();

    Move genmove(Color c);

    ValueType get_fixed_simulations() const;

    /** Use a fixed number of simulations in the search.
        If set to a value greater than zero, this value will enforce a
        fixed number of simulations per search independent of the playing
        level. */
    void set_fixed_simulations(ValueType n);

    bool get_use_book() const;

    void set_use_book(bool enable);

    int get_level() const;

    void set_level(int level);

    Search& get_search();

    void load_book(istream& in);

protected:
    bool m_is_book_loaded;

    bool m_use_book;

    int m_level;

    array<float, Board::max_player_moves> weight_max_count;

    ValueType m_fixed_simulations;

    Search m_search;

    Book m_book;
};

inline ValueType Player::get_fixed_simulations() const
{
    return m_fixed_simulations;
}

inline int Player::get_level() const
{
    return m_level;
}

inline bool Player::get_use_book() const
{
    return m_use_book;
}

inline Search& Player::get_search()
{
    return m_search;
}

inline void Player::set_fixed_simulations(ValueType n)
{
    m_fixed_simulations = n;
}

inline void Player::set_level(int level)
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
