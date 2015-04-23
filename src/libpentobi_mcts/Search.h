//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/Search.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_SEARCH_H
#define LIBPENTOBI_MCTS_SEARCH_H

#include "History.h"
#include "SearchParamConst.h"
#include "State.h"
#include "libboardgame_mcts/SearchBase.h"

namespace libpentobi_mcts {

using namespace std;
using libboardgame_mcts::PlayerInt;
using libboardgame_util::TimeSource;
using libpentobi_base::Setup;

//-----------------------------------------------------------------------------

/** Monte-Carlo tree search implementation for Blokus.
    Multiple colors per player (e.g. in Classic 2) are handled by using the
    same game result for each color of a player.
    Multiple players of a color (the 4th color in Classic 3) are handled by
    adding additional players for each player of this color that share the
    game result with the main color of the player.
    The maximum number of players is 6, which occurs in Classic 3 with 3
    real players and 3 pseudo-players for the 4th color.
    @note @ref libboardgame_avoid_stack_allocation */
class Search final
    : public libboardgame_mcts::SearchBase<State, Move, SearchParamConst>
{
public:
    Search(Variant initial_variant, unsigned nu_threads, size_t memory);

    ~Search();

    unique_ptr<State> create_state() override;

    PlayerInt get_nu_players() const override;

    PlayerInt get_player() const override;

    Float get_tie_value() const override;

    bool check_followup(ArrayList<Move, max_moves>& sequence) override;

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

    const History& get_last_history() const;

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
    History m_history;

    History m_last_history;

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

inline const History& Search::get_last_history() const
{
    return m_last_history;
}

inline PlayerInt Search::get_nu_players() const
{
    return m_variant != Variant::classic_3 ? get_board().get_nu_colors() : 6;
}

inline PlayerInt Search::get_player() const
{
    auto to_play = m_to_play.to_int();
    if ( m_variant == Variant::classic_3 && to_play == 3)
        return static_cast<PlayerInt>(to_play + get_board().get_alt_player());
    else
        return to_play;
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
