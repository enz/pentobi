//-----------------------------------------------------------------------------
/** @file libpentobi_base/MoveMarker.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MOVE_MARKER_H
#define LIBPENTOBI_BASE_MOVE_MARKER_H

#include <vector>
#include "Move.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

class MoveMarker
{
public:
    MoveMarker();

    void clear(Move mv);

    void clear(const vector<Move>& moves);

    void set(Move mv);

    bool operator[](Move mv) const;

private:
    vector<bool> m_marker;
};

inline MoveMarker::MoveMarker()
    : m_marker(Move::range, false)
{
}

inline bool MoveMarker::operator[](Move mv) const
{
    return m_marker[mv.to_int()];
}

inline void MoveMarker::clear(Move mv)
{
    m_marker[mv.to_int()] = false;
}

inline void MoveMarker::clear(const vector<Move>& moves)
{
    auto begin = moves.begin();
    auto end = moves.end();
    for (auto i = begin; i != end; ++i)
        clear(*i);
}

inline void MoveMarker::set(Move mv)
{
    m_marker[mv.to_int()] = true;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_MOVE_MARKER_H
