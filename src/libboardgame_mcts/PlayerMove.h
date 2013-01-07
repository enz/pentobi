//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/PlayerMove.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_PLAYER_MOVE_H
#define LIBBOARDGAME_MCTS_PLAYER_MOVE_H

namespace libboardgame_mcts {

//-----------------------------------------------------------------------------

typedef uint_fast8_t PlayerInt;

//-----------------------------------------------------------------------------

template<typename MOVE>
struct PlayerMove
{
    PlayerInt player;

    MOVE move;

    PlayerMove(PlayerInt player, MOVE move);
};

template<typename MOVE>
inline PlayerMove<MOVE>::PlayerMove(PlayerInt player, MOVE move)
{
    this->player = player;
    this->move = move;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_PLAYER_MOVE_H
