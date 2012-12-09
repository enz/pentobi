//-----------------------------------------------------------------------------
/** @file libpentobi_base/MoveMarker.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_MOVE_MARKER_H
#define LIBPENTOBI_BASE_MOVE_MARKER_H

#include <cstring>
#include <limits>
#include <vector>
#include "MoveList.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

/** Marker for moves.
    @note @ref libboardgame_avoid_stack_allocation */
class MoveMarker
{
public:
    MoveMarker();

    /** Clear all moves (slow). */
    void clear();

    void clear(Move mv);

    void clear(const vector<Move>& moves);

    /** Clear all moves from a list optimized for the case when we know
        that the list contains all marked moves in the marker.
        This function assumes that all moves not in the list are not marked
        and is faster than clear(ArrayList). */
    void clear_all_set_known(const MoveList& moves);

    void set(Move mv);

    /** Mark all moves (slow). */
    void set_all();

    bool operator[](Move mv) const;

    /** Mark a move and return whether it was already marked. */
    bool test_and_set(Move mv);

private:
    typedef unsigned Word;

    static const int word_bits = numeric_limits<Word>::digits;

    static const size_t array_size =
        Move::range / word_bits + (Move::range % word_bits == 0 ? 0 : 1);

    Word m_array[array_size];

    size_t get_index(Move mv) const;

    Word get_mask(Move mv) const;
};

inline MoveMarker::MoveMarker()
{
    clear();
}

inline bool MoveMarker::operator[](Move mv) const
{
    return ((m_array[get_index(mv)] & get_mask(mv)) != 0);
}

inline void MoveMarker::clear()
{
    memset(m_array, 0, array_size * sizeof(Word));
}

inline void MoveMarker::clear(Move mv)
{
    m_array[get_index(mv)] &= ~get_mask(mv);
}

inline void MoveMarker::clear(const vector<Move>& moves)
{
    auto begin = moves.begin();
    auto end = moves.end();
    for (auto i = begin; i != end; ++i)
        clear(*i);
}

inline void MoveMarker::clear_all_set_known(const MoveList& moves)
{
    auto begin = moves.begin();
    auto end = moves.end();
    for (auto i = begin; i != end; ++i)
        m_array[get_index(*i)] = 0;
}

inline size_t MoveMarker::get_index(Move mv) const
{
    return mv.to_int() / word_bits;
}

inline MoveMarker::Word MoveMarker::get_mask(Move mv) const
{
    return static_cast<Word>(1) << (mv.to_int() % word_bits);
}

inline void MoveMarker::set(Move mv)
{
    m_array[get_index(mv)] |= get_mask(mv);
}

inline void MoveMarker::set_all()
{
    memset(m_array, ~0, array_size * sizeof(Word));
}

inline bool MoveMarker::test_and_set(Move mv)
{
    Word& elem = m_array[get_index(mv)];
    Word mask = get_mask(mv);
    if (elem & mask)
        return true;
    elem |= mask;
    return false;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_MOVE_MARKER_H
