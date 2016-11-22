//-----------------------------------------------------------------------------
/** @file libboardgame_mcts/MoveHash.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBBOARDGAME_MCTS_MOVE_HASH_H
#define LIBBOARDGAME_MCTS_MOVE_HASH_H

#include <cstddef>
#include <random>

namespace libboardgame_mcts {

using namespace std;

//-----------------------------------------------------------------------------

/** Generates a hash value for a move.
    @tparam M The move type. */
template<class M>
class MoveHash
{
public:
    typedef M Move;

    MoveHash();

    size_t get(Move mv) const { return m_hash[mv.to_int()]; }

private:
    size_t m_hash[Move::range];

};

template<class M>
MoveHash<M>::MoveHash()
{
    mt19937 generator;
    for (auto& hash : m_hash)
        hash = generator();
}

//-----------------------------------------------------------------------------

} // namespace libboardgame_mcts

#endif // LIBBOARDGAME_MCTS_LAST_GOOD_REPLY_H
