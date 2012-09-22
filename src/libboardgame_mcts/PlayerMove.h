//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/PlayerMove.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_PLAYER_MOVE_H
#define LIBBOARDGAME_MCTS_PLAYER_MOVE_H

namespace libboardgame_mcts {

//-----------------------------------------------------------------------------

template<typename MOVE>
struct PlayerMove
{
    unsigned player;

    MOVE move;

    PlayerMove(unsigned player, MOVE move);
};

template<typename MOVE>
inline PlayerMove<MOVE>::PlayerMove(unsigned player, MOVE move)
{
    this->player = player;
    this->move = move;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_PLAYER_MOVE_H
