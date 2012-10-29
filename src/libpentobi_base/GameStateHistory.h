//----------------------------------------------------------------------------
/** @file libpentobi_base/GameStateHistory.h */
//----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_GAME_STATE_HISTORY_H
#define LIBPENTOBI_BASE_GAME_STATE_HISTORY_H

#include "Board.h"

namespace libpentobi_base {

using namespace std;

//----------------------------------------------------------------------------

/** Identifier for board state including history.
    This class can be used, for instance, to uniquely remember a board
    position for reusing parts of previous computations. The state includes:
    - the game variant
    - the history of moves
    - the color to play */
class GameStateHistory
{
public:
    /** Constructor.
        The initial state is that the history does not correspond to any
        valid position. */
    GameStateHistory();

    /** Initialize from a current board position. */
    void init(const Board& bd);

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
        this one
        @return @c true If the position is a followup
    */
    bool is_followup(const GameStateHistory& other,
                     vector<Move>& sequence) const;

    /** Get the position of the board state as setup.
        @pre is_valid()
        @param[out] variant
        @param[out] setup */
    void get_as_setup(Variant& variant, Setup& setup) const;

private:
    bool m_is_valid;

    Color::IntType m_nu_colors;

    Variant m_variant;

    vector<ColorMove> m_moves;

    Color m_to_play;
};

inline GameStateHistory::GameStateHistory()
{
    clear();
}

inline void GameStateHistory::clear()
{
    m_is_valid = false;
}

inline bool GameStateHistory::is_valid() const
{
    return m_is_valid;
}

//----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_GAME_STATE_HISTORY_H
