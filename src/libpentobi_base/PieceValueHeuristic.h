//-----------------------------------------------------------------------------
/** @file libpentobi_base/PieceValueHeuristic.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_PIECE_VALUE_HEURISTIC
#define LIBPENTOBI_BASE_PIECE_VALUE_HEURISTIC

#include <array>
#include <string>
#include "Board.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

/** A heuristic value that indicates how preferable a piece is to play early.
    The values are in [0..1] and the pieces are ordered by large size,
    large number of corners and low number of edges (in this order of
    importance). */
class PieceValueHeuristic
{
public:
    PieceValueHeuristic(const Board& bd);

    float get(unsigned int piece) const;

private:
    array<float, BoardConst::max_pieces> m_value;

    void set(const Board& bd, const string& name, float value);
};

inline float PieceValueHeuristic::get(unsigned int piece) const
{
    return m_value[piece];
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_PIECE_VALUE_HEURISTIC
