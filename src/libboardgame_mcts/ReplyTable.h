//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/ReplyTable.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_REPLY_TABLE_H
#define LIBBOARDGAME_MCTS_REPLY_TABLE_H

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include "PlayerMove.h"
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/Log.h"

namespace libboardgame_mcts {

using namespace std;
using libboardgame_util::log;

//-----------------------------------------------------------------------------

/** Stores last-good-reply heuristics for MCTS playouts.
    To save space, it assumes that the color of the next move is always the
    same for a given color of a previous move (checked with assertions).
    @see Search::set_last_good_reply() */
template<class S, class M, unsigned int P>
class ReplyTable
{
public:
    typedef M Move;

    static const unsigned int max_players = P;

    typedef libboardgame_mcts::PlayerMove<M> PlayerMove;

    void init(unsigned int nu_players);

    void store(PlayerMove mv, PlayerMove reply);

    void forget(PlayerMove mv, PlayerMove reply);

    Move get_reply(PlayerMove mv) const;

private:
    array<array<Move,Move::range>,max_players> m_reply;

#if LIBBOARDGAME_DEBUG
    unsigned int m_nu_players;

    array<unsigned int,max_players> m_next_player;

    array<bool,max_players> m_is_next_player_known;
#endif
};

template<class S, class M, unsigned int P>
void ReplyTable<S,M,P>::init(unsigned int nu_players)
{
#if LIBBOARDGAME_DEBUG
    m_nu_players = nu_players;
    for (unsigned int i = 0; i < nu_players; ++i)
        m_is_next_player_known[i] = false;
#endif
    for (unsigned int i = 0; i < nu_players; ++i)
        fill(m_reply[i].begin(), m_reply[i].end(), Move::null());
}

template<class S, class M, unsigned int P>
void ReplyTable<S,M,P>::store(PlayerMove mv, PlayerMove reply)
{
    LIBBOARDGAME_ASSERT(mv.player < m_nu_players);
#if LIBBOARDGAME_DEBUG
    if (m_is_next_player_known[mv.player])
        LIBBOARDGAME_ASSERT(m_next_player[mv.player] == reply.player);
    else
    {
        m_next_player[mv.player] = reply.player;
        m_is_next_player_known[mv.player] = true;
    }
#endif
    m_reply[mv.player][mv.move.to_int()] = reply.move;
}

template<class S, class M, unsigned int P>
void ReplyTable<S,M,P>::forget(PlayerMove mv, PlayerMove reply)
{
    LIBBOARDGAME_UNUSED_IF_NOT_DEBUG(reply);
    LIBBOARDGAME_ASSERT(mv.player < m_nu_players);
#if LIBBOARDGAME_DEBUG
    if (m_is_next_player_known[mv.player])
        LIBBOARDGAME_ASSERT(m_next_player[mv.player] == reply.player);
    else
    {
        m_next_player[mv.player] = reply.player;
        m_is_next_player_known[mv.player] = true;
    }
#endif
    LIBBOARDGAME_ASSERT(m_reply[mv.player][mv.move.to_int()] == reply.move);
    m_reply[mv.player][mv.move.to_int()] = Move::null();
}

template<class S, class M, unsigned int P>
M ReplyTable<S,M,P>::get_reply(PlayerMove mv) const
{
    LIBBOARDGAME_ASSERT(mv.player < m_nu_players);
    return m_reply[mv.player][mv.move.to_int()];
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_REPLY_TABLE_H
