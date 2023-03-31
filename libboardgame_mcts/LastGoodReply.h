//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/LastGoodReply.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_LAST_GOOD_REPLY_H
#define LIBBOARDGAME_MCTS_LAST_GOOD_REPLY_H

#include <cstddef>
#include <random>
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
    using Move = M;


    static constexpr unsigned max_players = P;

    static constexpr size_t hash_table_size = S;


    LastGoodReply();

    void init(PlayerInt nu_players);

    void store(PlayerInt player, Move last, Move second_last, Move reply);

    void forget(PlayerInt player, Move last, Move second_last, Move reply);

    Move get_lgr1(PlayerInt player, Move last) const;

    Move get_lgr2(PlayerInt player, Move last, Move second_last) const;

private:
    size_t m_hash1[Move::range];

    size_t m_hash2[Move::range];

    Atomic<typename Move::IntType, MT> m_lgr1[max_players][Move::range];

    Atomic<typename Move::IntType, MT> m_lgr2[max_players][hash_table_size];

    size_t get_index(Move last, Move second_last) const;
};

template<class M, unsigned P, size_t S, bool MT>
LastGoodReply<M, P, S, MT>::LastGoodReply()
{
    mt19937 generator;
    for (auto& hash : m_hash1)
        hash = generator();
    for (auto& hash : m_hash2)
        hash = generator();
}

template<class M, unsigned P, size_t S, bool MT>
inline size_t LastGoodReply<M, P, S, MT>::get_index(Move last,
                                                    Move second_last) const
{
    size_t hash = (m_hash1[last.to_int()] ^ m_hash2[second_last.to_int()]);
    return hash % hash_table_size;
}

template<class M, unsigned P, size_t S, bool MT>
inline auto LastGoodReply<M, P, S, MT>::get_lgr1(PlayerInt player,
                                                 Move last) const -> Move
{
    return Move(m_lgr1[player][last.to_int()].load(memory_order_relaxed));
}

template<class M, unsigned P, size_t S, bool MT>
inline auto LastGoodReply<M, P, S, MT>::get_lgr2(
        PlayerInt player, Move last, Move second_last) const -> Move
{
    auto index = get_index(last, second_last);
    return Move(m_lgr2[player][index].load(memory_order_relaxed));
}

template<class M, unsigned P, size_t S, bool MT>
void LastGoodReply<M, P, S, MT>::init(PlayerInt nu_players)
{
    for (PlayerInt i = 0; i < nu_players; ++i)
    {
        for (typename Move::IntType j = 0; j < Move::range; ++j)
            m_lgr1[i][j].store(Move::null().to_int(), memory_order_relaxed);
        for (size_t j = 0; j < hash_table_size; ++j)
            m_lgr2[i][j].store(Move::null().to_int(), memory_order_relaxed);
    }
}

template<class M, unsigned P, size_t S, bool MT>
inline void LastGoodReply<M, P, S, MT>::forget(PlayerInt player, Move last,
                                               Move second_last, Move reply)
{
    auto reply_int = reply.to_int();
    auto null_int = Move::null().to_int();
    {
        auto index = get_index(last, second_last);
        auto& stored_reply = m_lgr2[player][index];
        if (stored_reply.load(memory_order_relaxed) == reply_int)
            stored_reply.store(null_int, memory_order_relaxed);
    }
    auto& stored_reply = m_lgr1[player][last.to_int()];
    if (stored_reply.load(memory_order_relaxed) == reply_int)
        stored_reply.store(null_int, memory_order_relaxed);
}

template<class M, unsigned P, size_t S, bool MT>
inline void LastGoodReply<M, P, S, MT>::store(PlayerInt player, Move last,
                                              Move second_last, Move reply)
{
    auto reply_int = reply.to_int();
    auto index = get_index(last, second_last);
    m_lgr2[player][index].store(reply_int, memory_order_relaxed);
    m_lgr1[player][last.to_int()].store(reply_int, memory_order_relaxed);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_LAST_GOOD_REPLY_H
