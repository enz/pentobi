//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/LastGoodReply.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_LAST_GOOD_REPLY_H
#define LIBBOARDGAME_MCTS_LAST_GOOD_REPLY_H

#include <algorithm>
#include <atomic>
#include <memory>
#include "PlayerMove.h"
#include "libboardgame_util/Assert.h"
#include "libboardgame_util/RandomGenerator.h"

namespace libboardgame_mcts {

using namespace std;
using libboardgame_util::RandomGenerator;

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
template<class S, class M, unsigned P>
class LastGoodReply
{
public:
    typedef M Move;

    static const unsigned max_players = P;

    LastGoodReply();

    void init(PlayerInt nu_players);

    void store(PlayerInt player, Move last_mv, Move second_last_mv, Move reply);

    void store(PlayerInt player, Move last_mv, Move reply);

    void forget(PlayerInt player, Move last_mv, Move second_last_mv,
                Move reply);

    void forget(PlayerInt player, Move last_mv, Move reply);

    void get(PlayerInt player, Move last_mv, Move second_last_mv,
             Move& last_good_reply_1, Move& last_good_reply_2) const;

private:
    static const size_t hash_table_size = (1 << 21);

    size_t m_hash[Move::range];

    atomic<typename Move::IntType> m_reply_1[max_players][Move::range];

    atomic<typename Move::IntType> m_reply_2[max_players][hash_table_size];

    size_t get_index(Move last_mv, Move second_last_mv) const;
};

template<class S, class M, unsigned P>
LastGoodReply<S,M,P>::LastGoodReply()
{
    RandomGenerator generator;
    for (unsigned i = 0; i < Move::range; ++i)
        m_hash[i] = generator.generate();
}

template<class S, class M, unsigned P>
inline size_t LastGoodReply<S,M,P>::get_index(Move last_mv,
                                              Move second_last_mv) const
{
    size_t hash = (m_hash[last_mv.to_int()] ^ m_hash[second_last_mv.to_int()]);
    return hash % hash_table_size;
}

template<class S, class M, unsigned P>
inline void LastGoodReply<S,M,P>::get(PlayerInt player, Move last_mv,
                                      Move second_last_mv,
                                      Move& last_good_reply_1,
                                      Move& last_good_reply_2) const
{
    LIBBOARDGAME_ASSERT(! last_mv.is_null());
    if (! second_last_mv.is_null())
    {
        auto index = get_index(last_mv, second_last_mv);
        last_good_reply_2 =
            Move(m_reply_2[player][index].load(memory_order_relaxed));
    }
    else
        last_good_reply_2 = Move::null();
    last_good_reply_1 =
        Move(m_reply_1[player][last_mv.to_int()].load(memory_order_relaxed));
}

template<class S, class M, unsigned P>
void LastGoodReply<S,M,P>::init(PlayerInt nu_players)
{
    for (PlayerInt i = 0; i < nu_players; ++i)
    {
        auto null_int = Move::null().to_int();
        // Don't use memory_order_relaxed here. init() could be called after
        // the game variant changed (e.g. board size) and while this class
        // does not guarantee that a move is legal in the current position,
        // it should at least only return moves that belong to the same game
        // variant.
        fill(m_reply_1[i], m_reply_1[i] + Move::range, null_int);
        fill(m_reply_2[i], m_reply_2[i] + hash_table_size, null_int);
    }
}

template<class S, class M, unsigned P>
inline void LastGoodReply<S,M,P>::forget(PlayerInt player, Move last_mv,
                                         Move second_last_mv, Move reply)
{
    LIBBOARDGAME_ASSERT(! last_mv.is_null());
    LIBBOARDGAME_ASSERT(! second_last_mv.is_null());
    auto reply_int = reply.to_int();
    auto null_int = Move::null().to_int();
    {
        auto index = get_index(last_mv, second_last_mv);
        auto& stored_reply = m_reply_2[player][index];
        if (stored_reply.load(memory_order_relaxed) == reply_int)
            stored_reply.store(null_int, memory_order_relaxed);
    }
    auto& stored_reply = m_reply_1[player][last_mv.to_int()];
    if (stored_reply.load(memory_order_relaxed) == reply_int)
        stored_reply.store(null_int, memory_order_relaxed);
}

template<class S, class M, unsigned P>
inline void LastGoodReply<S,M,P>::forget(PlayerInt player, Move last_mv,
                                         Move reply)
{
    LIBBOARDGAME_ASSERT(! last_mv.is_null());
    auto reply_int = reply.to_int();
    auto null_int = Move::null().to_int();
    auto& stored_reply = m_reply_1[player][last_mv.to_int()];
    if (stored_reply.load(memory_order_relaxed) == reply_int)
        stored_reply.store(null_int, memory_order_relaxed);
}

template<class S, class M, unsigned P>
inline void LastGoodReply<S,M,P>::store(PlayerInt player, Move last_mv,
                                        Move second_last_mv, Move reply)
{
    LIBBOARDGAME_ASSERT(! last_mv.is_null());
    LIBBOARDGAME_ASSERT(! second_last_mv.is_null());
    auto reply_int = reply.to_int();
    {
        auto index = get_index(last_mv, second_last_mv);
        m_reply_2[player][index].store(reply_int, memory_order_relaxed);
    }
    m_reply_1[player][last_mv.to_int()].store(reply_int, memory_order_relaxed);
}

template<class S, class M, unsigned P>
inline void LastGoodReply<S,M,P>::store(PlayerInt player, Move last_mv,
                                        Move reply)
{
    LIBBOARDGAME_ASSERT(! last_mv.is_null());
    auto reply_int = reply.to_int();
    m_reply_1[player][last_mv.to_int()].store(reply_int, memory_order_relaxed);
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_LAST_GOOD_REPLY_H
