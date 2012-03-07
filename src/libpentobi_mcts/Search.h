//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/Search.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_SEARCH_H
#define LIBPENTOBI_MCTS_SEARCH_H

#include "State.h"
#include "libboardgame_mcts/Search.h"
#include "libpentobi_base/GameStateHistory.h"

namespace libpentobi_mcts {

using libboardgame_util::Timer;
using libboardgame_util::TimeSource;
using libpentobi_base::GameStateHistory;

//-----------------------------------------------------------------------------

class Search
    : public libboardgame_mcts::Search<State, Move, 4>
{
public:
    Search(GameVariant initial_game_variant);

    ~Search() throw();


    /** @name Pure virtual functions of libboardgame_mcts::Search */
    // @{

    string get_move_string(Move mv) const;

    unsigned int get_nu_players() const;

    unsigned int get_player() const;

    // @} // @name


    /** @name Overriden virtual functions of libboardgame_mcts::Search */
    // @{

    bool check_followup(vector<Move>& sequence);

    void write_info(ostream& out) const;

    // @} // @name


    /** @name Parameters */
    // @{

    ValueType get_score_modification() const;

    void set_score_modification(ValueType value);

    bool get_detect_symmetry() const;

    void set_detect_symmetry(bool enable);

    bool get_avoid_symmetric_draw() const;

    void set_avoid_symmetric_draw(bool enable);

    // @} // @name


    bool search(Move& mv, const Board& bd, Color to_play, ValueType max_count,
                size_t min_simulations, double max_time,
                TimeSource& time_source);

    /** Get color to play at root node of the last search. */
    Color get_to_play() const;

protected:
    void on_start_search();

private:
    typedef libboardgame_mcts::Search<State, Move, 4> ParentClass;

    Color m_to_play;

    SharedConst m_shared_const;

    /** Local variable reused for efficiency. */
    GameStateHistory m_state;

    GameStateHistory m_last_state;

    const Board& get_board() const;
};

inline bool Search::get_avoid_symmetric_draw() const
{
    return m_shared_const.avoid_symmetric_draw;
}

inline const Board& Search::get_board() const
{
    return *m_shared_const.board;
}

inline bool Search::get_detect_symmetry() const
{
    return m_shared_const.detect_symmetry;
}

inline unsigned int Search::get_nu_players() const
{
    return get_board().get_nu_colors();
}

inline unsigned int Search::get_player() const
{
    return m_to_play.to_int();
}

inline ValueType Search::get_score_modification() const
{
    return m_shared_const.score_modification;
}

inline Color Search::get_to_play() const
{
    return m_to_play;
}

inline void Search::set_avoid_symmetric_draw(bool enable)
{
    m_shared_const.avoid_symmetric_draw = enable;
}

inline void Search::set_detect_symmetry(bool enable)
{
    m_shared_const.detect_symmetry = enable;
}

inline void Search::set_score_modification(ValueType value)
{
    m_shared_const.score_modification = value;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_SEARCH_H
