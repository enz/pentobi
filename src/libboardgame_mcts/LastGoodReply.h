//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/LastGoodReply.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_LAST_GOOD_REPLY_H
#define LIBBOARDGAME_MCTS_LAST_GOOD_REPLY_H

#include <algorithm>
#include <memory>
#include <random>
#include "Atomic.h"
#include "PlayerMove.h"
#include "libboardgame_util/Assert.h"

namespace libboardgame_mcts {

using namespace std;

//-----------------------------------------------------------------------------

/** Storage for Last-Good-Reply heuristic.
    Uses LGRF-2 (Baier, Drake: The Power of Forgetting: Improving the
    Last-Good-Reply Policy in Monte-Carlo Go. 2010.
    http://webdisk.lclark.edu/drake/publications/baier-drake-ieee-2010.pdf)
    To save space, only the player of the reply move is considered when storing
    or receiving a reply, the players of the last and second last moves are
    ignored. In games without a fixed order of players (i.e. when move
    sequences with the same moves but not played by the same players occur),
    this can cause undetected collisions. If these collisions are not
    sufficiently rare, the last-good-reply heuristic should be disabled in the
    search. Undetected collisions can also occur because the replies are stored
    in a hash table without collision check. But since the replies have to be
    checked for legality in the current position anyway and the collisions are
    probably rare, no major negative effect is expected from these collisions.
    @see Search::set_last_good_reply() */
template<class M, unsigned P>
class LastGoodReply
{
public:
    typedef M Move;

    static const unsigned max_players = P;

    LastGoodReply();

    void init(PlayerInt nu_players);

    void store(PlayerInt player, Move last_mv, Move second_last_mv,
               Move reply);

    void forget(PlayerInt player, Move last_mv, Move second_last_mv,
                Move reply);

    void get(PlayerInt player, Move last_mv, Move second_last_mv,
             Move& lgr1, Move& lgr2) const;

private:
    static const size_t hash_table_size = (1 << 21);

    size_t m_hash[Move::range];

    LIBBOARDGAME_MCTS_ATOMIC(typename Move::IntType)
        m_lgr1[max_players][Move::range];

    LIBBOARDGAME_MCTS_ATOMIC(typename Move::IntType)
        m_lgr2[max_players][hash_table_size];

    size_t get_index(Move last_mv, Move second_last_mv) const;
};

template<class M, unsigned P>
LastGoodReply<M, P>::LastGoodReply()
{
    mt19937 generator;
    for (auto& hash : m_hash)
        hash = generator();
}

template<class M, unsigned P>
inline size_t LastGoodReply<M, P>::get_index(Move last_mv,
                                             Move second_last_mv) const
{
    size_t hash = (m_hash[last_mv.to_int()] ^ m_hash[second_last_mv.to_int()]);
    return hash % hash_table_size;
}

template<class M, unsigned P>
inline void LastGoodReply<M, P>::get(PlayerInt player, Move last_mv,
                                     Move second_last_mv,
                                     Move& lgr1, Move& lgr2) const
{
    auto index = get_index(last_mv, second_last_mv);
    lgr2 = Move(LIBBOARDGAME_MCTS_ATOMIC_LOAD(m_lgr2[player][index],
                                              memory_order_relaxed));
    lgr1 = Move(LIBBOARDGAME_MCTS_ATOMIC_LOAD(m_lgr1[player][last_mv.to_int()],
                                              memory_order_relaxed));
}

template<class M, unsigned P>
void LastGoodReply<M, P>::init(PlayerInt nu_players)
{
    for (PlayerInt i = 0; i < nu_players; ++i)
    {
        auto null_int = Move::null().to_int();
        fill(m_lgr1[i], m_lgr1[i] + Move::range, null_int);
        fill(m_lgr2[i], m_lgr2[i] + hash_table_size, null_int);
    }
}

template<class M, unsigned P>
inline void LastGoodReply<M, P>::forget(PlayerInt player, Move last_mv,
                                        Move second_last_mv, Move reply)
{
    auto reply_int = reply.to_int();
    auto null_int = Move::null().to_int();
    {
        auto index = get_index(last_mv, second_last_mv);
        auto& stored_reply = m_lgr2[player][index];
        if (LIBBOARDGAME_MCTS_ATOMIC_LOAD(stored_reply, memory_order_relaxed)
            == reply_int)
            LIBBOARDGAME_MCTS_ATOMIC_STORE(stored_reply, null_int,
                                           memory_order_relaxed);
    }
    auto& stored_reply = m_lgr1[player][last_mv.to_int()];
    if (LIBBOARDGAME_MCTS_ATOMIC_LOAD(stored_reply, memory_order_relaxed)
        == reply_int)
        LIBBOARDGAME_MCTS_ATOMIC_STORE(stored_reply, null_int,
                                       memory_order_relaxed);
}

template<class M, unsigned P>
inline void LastGoodReply<M, P>::store(PlayerInt player, Move last_mv,
                                       Move second_last_mv, Move reply)
{
    auto reply_int = reply.to_int();
    {
        auto index = get_index(last_mv, second_last_mv);
        LIBBOARDGAME_MCTS_ATOMIC_STORE(m_lgr2[player][index], reply_int,
                                       memory_order_relaxed);
    }
    LIBBOARDGAME_MCTS_ATOMIC_STORE(m_lgr1[player][last_mv.to_int()], reply_int,
                                   memory_order_relaxed);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_LAST_GOOD_REPLY_H
