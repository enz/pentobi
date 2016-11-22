//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/LastGoodReply.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_LAST_GOOD_REPLY_H
#define LIBBOARDGAME_MCTS_LAST_GOOD_REPLY_H

#include <algorithm>
#include <cstring>
#include "Atomic.h"
#include "PlayerMove.h"

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
    @tparam M The move type.
    @tparam P The (maximum) number of players.
    @tparam S The number of entries in the LGR2 has table (per player).
    @tparam MT Whether the LGR table is used in a multi-threaded search. */
template<class M, unsigned P, size_t S, bool MT>
class LastGoodReply
{
public:
    typedef M Move;

    static const unsigned max_players = P;

    static const size_t hash_table_size = S;


    void init(PlayerInt nu_players);

    void store(PlayerInt player, size_t last_hash, size_t second_last_hash,
               Move reply);

    void forget(PlayerInt player, size_t last_hash, size_t second_last_hash,
                Move reply);

    Move get_lgr1(PlayerInt player, size_t last_hash) const;

    Move get_lgr2(PlayerInt player, size_t last_hash,
                  size_t second_last_hash) const;

private:
    Atomic<typename Move::IntType, MT> m_table[max_players][hash_table_size];


    size_t get_index(size_t last_hash) const;

    size_t get_index(size_t last_hash, size_t second_last_hash) const;
};


template<class M, unsigned P, size_t S, bool MT>
inline size_t LastGoodReply<M, P, S, MT>::get_index(size_t last_hash) const
{
    return last_hash % hash_table_size;
}

template<class M, unsigned P, size_t S, bool MT>
inline size_t LastGoodReply<M, P, S, MT>::get_index(
        size_t last_hash, size_t second_last_hash) const
{
    auto hash = (last_hash ^ second_last_hash);
    return hash % hash_table_size;
}

template<class M, unsigned P, size_t S, bool MT>
inline auto LastGoodReply<M, P, S, MT>::get_lgr1(
        PlayerInt player, size_t last_hash) const -> Move
{
    auto index = get_index(last_hash);
    return Move(m_table[player][index].load(memory_order_relaxed));
}

template<class M, unsigned P, size_t S, bool MT>
inline auto LastGoodReply<M, P, S, MT>::get_lgr2(
        PlayerInt player, size_t last_hash,
        size_t second_last_hash) const -> Move
{
    auto index = get_index(last_hash, second_last_hash);
    return Move(m_table[player][index].load(memory_order_relaxed));
}

template<class M, unsigned P, size_t S, bool MT>
void LastGoodReply<M, P, S, MT>::init(PlayerInt nu_players)
{
    for (PlayerInt i = 0; i < nu_players; ++i)
        if (Move::null().to_int() == 0)
            // Using memset is ok even if the elements are atomic because
            // init() is used before the multi-threaded search starts.
            memset(m_table[i], 0, hash_table_size * sizeof(m_table[i][0]));
        else
            fill(m_table[i], m_table[i] + hash_table_size,
                 Move::null().to_int());
}

template<class M, unsigned P, size_t S, bool MT>
inline void LastGoodReply<M, P, S, MT>::forget(
        PlayerInt player, size_t last_hash, size_t second_last_hash,
        Move reply)
{
    auto reply_int = reply.to_int();
    auto null_int = Move::null().to_int();
    {
        auto index = get_index(last_hash, second_last_hash);
        auto& stored_reply = m_table[player][index];
        if (stored_reply.load(memory_order_relaxed) == reply_int)
            stored_reply.store(null_int, memory_order_relaxed);
    }
    auto& stored_reply = m_table[player][get_index(last_hash)];
    if (stored_reply.load(memory_order_relaxed) == reply_int)
        stored_reply.store(null_int, memory_order_relaxed);
}

template<class M, unsigned P, size_t S, bool MT>
inline void LastGoodReply<M, P, S, MT>::store(
        PlayerInt player, size_t last_hash, size_t second_last_hash,
        Move reply)
{
    auto reply_int = reply.to_int();
    auto index = get_index(last_hash, second_last_hash);
    m_table[player][index].store(reply_int, memory_order_relaxed);
    m_table[player][get_index(last_hash)]
            .store(reply_int, memory_order_relaxed);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_LAST_GOOD_REPLY_H
