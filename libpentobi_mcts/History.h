//-----------------------------------------------------------------------------
/** @file libpentobi_mcts/History.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_MCTS_HISTORY_H
#define LIBPENTOBI_MCTS_HISTORY_H

#include "SearchParamConst.h"
#include "libpentobi_base/Board.h"

namespace libpentobi_mcts {

using libboardgame_base::ArrayList;
using libpentobi_base::ColorMove;
using libpentobi_base::Move;
using libpentobi_base::Setup;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

/** Identifier for board state including history.
    This class can be used, for instance, to uniquely remember a board
    position for reusing parts of previous computations. The state includes:
    - the game variant
    - the history of moves
    - the color to play */
class History
{
public:
    /** Constructor.
        The initial state is that the history does not correspond to any
        valid position. */
    History();

    /** Initialize from a current board position and explicit color to play. */
    void init(const Board& bd, Color to_play);

    /** Clear the state.
        A cleared state does not correspond to any valid position. */
    void clear();

    /** Check if the state corresponds to any valid position. */
    bool is_valid() const;

    /** Check if this position is a alternate-play followup to another one.
        @param other The other position
        @param[out] sequence The sequence leading from the other position to
        this one. Pass (=null) moves are inserted to ensure alternating colors
        (as required by libpentobi_mcts::Search.)
        @return @c true If the position is a followup
    */
    bool is_followup(
            const History& other,
            ArrayList<Move, SearchParamConst::max_moves>& sequence) const;

    /** Get the position of the board state as setup.
        @pre is_valid()
        @param[out] variant
        @param[out] setup */
    void get_as_setup(Variant& variant, Setup& setup) const;

    Color get_to_play() const;

private:
    bool m_is_valid;

    Color::IntType m_nu_colors;

    Variant m_variant;

    ArrayList<ColorMove, Board::max_moves> m_moves;

    Color m_to_play;
};

inline History::History()
{
    clear();
}

inline void History::clear()
{
    m_is_valid = false;
}

inline Color History::get_to_play() const
{
    LIBBOARDGAME_ASSERT(m_is_valid);
    return m_to_play;
}

inline bool History::is_valid() const
{
    return m_is_valid;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_mcts

#endif // LIBPENTOBI_MCTS_HISTORY_H
