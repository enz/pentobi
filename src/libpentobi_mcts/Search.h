//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/Search.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_SEARCH_H
#define LIBPENTOBI_MCTS_SEARCH_H

#include "State.h"
#include "libboardgame_mcts/SearchBase.h"
#include "libpentobi_base/GameStateHistory.h"

namespace libpentobi_mcts {

using namespace std;
using libboardgame_mcts::PlayerInt;
using libboardgame_mcts::SearchBase;
using libboardgame_util::Timer;
using libboardgame_util::TimeSource;
using libpentobi_base::GameStateHistory;
using libpentobi_base::Setup;

//-----------------------------------------------------------------------------

/** Optional compile-time parameters for libboardgame_mcts::Search.
    See libboardgame_mcts::SearchParamConstDefault for the meaning of the
    members. */
struct SearchParamConst
{
    typedef libpentobi_mcts::Float Float;

    static const PlayerInt max_players = 4;

    static const bool rave = true;

    static const bool rave_check_same = false;

    static const bool rave_dist_weighting = true;

    static const bool use_last_good_reply = true;

    static const bool virtual_loss = true;
};

//-----------------------------------------------------------------------------

/** Monte-Carlo tree search implementation for Blokus.
    @note @ref libboardgame_avoid_stack_allocation */
class Search
    : public SearchBase<State, Move, SearchParamConst>
{
public:
    Search(Variant initial_variant, unsigned nu_threads, size_t memory);

    ~Search();

    unique_ptr<State> create_state() override;

    string get_move_string(Move mv) const override;

    PlayerInt get_nu_players() const override;

    PlayerInt get_player() const override;

    Float get_tie_value() const override;

    bool check_followup(vector<Move>& sequence) override;

    string get_info() const override;


    /** @name Parameters */
    /** @{ */

    bool get_avoid_symmetric_draw() const;

    void set_avoid_symmetric_draw(bool enable);

    /** Automatically set some user-changeable parameters that have different
        optimal values for different game variants whenever the game variant
        changes.
        Default is true. */
    bool get_auto_param() const;

    void set_auto_param(bool enable);

    /** @} */ // @name


    bool search(Move& mv, const Board& bd, Color to_play, Float max_count,
                Float min_simulations, double max_time,
                TimeSource& time_source);

    /** Get color to play at root node of the last search. */
    Color get_to_play() const;

    const GameStateHistory& get_last_state() const;

    /** Get board position of last search at root node as setup.
        @param[out] variant
        @param[out] setup */
    void get_root_position(Variant& variant, Setup& setup) const;

protected:
    void on_start_search(bool is_followup) override;

private:
    /** Automatically set default parameters for the game variant if
        the game variant changes. */
    bool m_auto_param;

    /** Game variant of last search. */
    Variant m_variant;

    Color m_to_play;

    SharedConst m_shared_const;

    /** Local variable reused for efficiency. */
    GameStateHistory m_state;

    GameStateHistory m_last_state;

    const Board& get_board() const;

    void set_default_param(Variant variant);
};

inline bool Search::get_auto_param() const
{
    return m_auto_param;
}

inline bool Search::get_avoid_symmetric_draw() const
{
    return m_shared_const.avoid_symmetric_draw;
}

inline const Board& Search::get_board() const
{
    return *m_shared_const.board;
}

inline const GameStateHistory& Search::get_last_state() const
{
    return m_last_state;
}

inline PlayerInt Search::get_nu_players() const
{
    return get_board().get_nu_colors();
}

inline PlayerInt Search::get_player() const
{
    return m_to_play.to_int();
}

inline Color Search::get_to_play() const
{
    return m_to_play;
}

inline void Search::set_auto_param(bool enable)
{
    m_auto_param = enable;
}

inline void Search::set_avoid_symmetric_draw(bool enable)
{
    m_shared_const.avoid_symmetric_draw = enable;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_SEARCH_H
