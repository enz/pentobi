//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/LastGoodReply.h */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_LAST_GOOD_REPLY_H
#define LIBBOARDGAME_MCTS_LAST_GOOD_REPLY_H

#include <algorithm>
#include <array>
#include <iosfwd>
#include <memory>
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

    void init(unsigned nu_players);

    void store(unsigned player, Move last_mv, Move second_last_mv, Move reply);

    void forget(unsigned player, Move last_mv, Move second_last_mv, Move reply);

    Move get(unsigned player, Move last_mv, Move second_last_mv) const;

private:
    static const size_t hash_table_size = (1 << 21);

    size_t m_hash[Move::range];

    Move m_reply_1[max_players][Move::range];

    Move m_reply_2[max_players][hash_table_size];

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
inline M LastGoodReply<S,M,P>::get(unsigned player, Move last_mv,
                                   Move second_last_mv) const
{
    LIBBOARDGAME_ASSERT(! last_mv.is_null());
    Move reply;
    if (! second_last_mv.is_null())
    {
        reply = m_reply_2[player][get_index(last_mv, second_last_mv)];
        if (! reply.is_null())
            return reply;
    }
    return m_reply_1[player][last_mv.to_int()];
}

template<class S, class M, unsigned P>
void LastGoodReply<S,M,P>::init(unsigned nu_players)
{
    for (unsigned i = 0; i < nu_players; ++i)
    {
        fill(m_reply_1[i], m_reply_1[i] + Move::range, Move::null());
        fill(m_reply_2[i], m_reply_2[i] + hash_table_size, Move::null());
    }
}

template<class S, class M, unsigned P>
inline void LastGoodReply<S,M,P>::forget(unsigned player, Move last_mv,
                                         Move second_last_mv, Move reply)
{
    LIBBOARDGAME_ASSERT(! last_mv.is_null());
    if (! second_last_mv.is_null())
    {
        Move& stored_reply =
            m_reply_2[player][get_index(last_mv, second_last_mv)];
        if (stored_reply == reply)
            stored_reply = Move::null();
    }
    Move& stored_reply = m_reply_1[player][last_mv.to_int()];
    if (stored_reply == reply)
        stored_reply = Move::null();
}

template<class S, class M, unsigned P>
inline void LastGoodReply<S,M,P>::store(unsigned player, Move last_mv,
                                        Move second_last_mv, Move reply)
{
    LIBBOARDGAME_ASSERT(! last_mv.is_null());
    if (! second_last_mv.is_null())
        m_reply_2[player][get_index(last_mv, second_last_mv)] = reply;
    m_reply_1[player][last_mv.to_int()] = reply;
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_LAST_GOOD_REPLY_H
