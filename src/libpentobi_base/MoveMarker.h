//-----------------------------------------------------------------------------
/** @file libpentobi_base/MoveMarker.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MOVE_MARKER_H
#define LIBPENTOBI_BASE_MOVE_MARKER_H

#include <bitset>
#include "Move.h"
#include "libboardgame_util/ArrayList.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_util::ArrayList;

//-----------------------------------------------------------------------------

class MoveMarker
{
public:
    MoveMarker();

    void clear(Move mv);

    void clear(const vector<Move>& moves);

    template<unsigned int M>
    void clear(const ArrayList<Move, M>& moves);

    void set(Move mv);

    /** Mark all moves (slow). */
    void set_all();

    bool operator[](Move mv) const;

private:
    bitset<Move::range> m_marker;
};

inline MoveMarker::MoveMarker()
{
}

inline bool MoveMarker::operator[](Move mv) const
{
    return m_marker.test(mv.to_int());
}

inline void MoveMarker::clear(Move mv)
{
    m_marker.reset(mv.to_int());
}

inline void MoveMarker::clear(const vector<Move>& moves)
{
    auto begin = moves.begin();
    auto end = moves.end();
    for (auto i = begin; i != end; ++i)
        clear(*i);
}

template<unsigned int M>
inline void MoveMarker::clear(const ArrayList<Move, M>& moves)
{
    auto begin = moves.begin();
    auto end = moves.end();
    for (auto i = begin; i != end; ++i)
        clear(*i);
}

inline void MoveMarker::set(Move mv)
{
    m_marker.set(mv.to_int());
}

inline void MoveMarker::set_all()
{
    m_marker.set();
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_MOVE_MARKER_H
