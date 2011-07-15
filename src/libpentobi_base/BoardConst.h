//-----------------------------------------------------------------------------
/** @file BoardConst.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOARD_CONST_H
#define LIBPENTOBI_BASE_BOARD_CONST_H

#include <array>
#include <algorithm>
#include "Grid.h"
#include "Marker.h"
#include "Move.h"
#include "MoveInfo.h"
#include "Point.h"
#include "Piece.h"
#include "libpentobi_base/Color.h"
#include "libpentobi_base/ColorMap.h"
#include "libboardgame_util/ArrayList.h"

namespace libpentobi_base {

using libboardgame_util::ArrayList;

//-----------------------------------------------------------------------------

/** Constant precomputed data that is shared between all instances of Board. */
class BoardConst
{
public:
    static const unsigned int nu_pieces = 21;

    static const unsigned int total_piece_points = 89;

    BoardConst(unsigned int sz);

    /** Get the single instance for a given board size.
        The instance is created the first time this function is called. */
    static const BoardConst& get(unsigned int size);

    const Piece& get_piece(unsigned int n) const;

    bool get_piece_index_by_name(const string& name,
                                 unsigned int& index) const;

    const MoveInfo& get_move_info(Move move) const;

    const MovePoints& get_move_points(Move mv) const;

    bool find_move(const MovePoints& points, Move& move) const;

    /** Get all moves of a piece at a point. */
    const vector<Move>& get_moves(unsigned int piece, Point p) const;

    /** Get all moves of a piece at a point constrained by a diagonal point.
        Get all moves of a piece that include a point and are legal if a
        diagonally adjacent point to the given point is occupied by the same
        color. */
    const vector<Move>& get_moves_diag(unsigned int piece, Point p,
                                       unsigned int diag_dir) const;

private:
    unsigned int m_sz;

    vector<Piece> m_pieces;

    vector<MoveInfo> m_move_info;

    array<Grid<vector<Move>>, nu_pieces> m_moves_at;

    array<array<Grid<vector<Move>>, nu_pieces>, 4> m_moves_at_diag;

    /** Local variable reused for efficiency. */
    Marker m_marker;

    void create_move(unsigned int piece, const Piece::Points& coord_points,
                     CoordPoint center, unsigned int x, unsigned int y);

    void create_moves(unsigned int piece);

    void init_symmetry_info();

    bool is_compatible_with_diag(Point p, Direction dir,
                                 const MovePoints& points);

    void set_adj_and_corner_points(MoveInfo& info);

    void set_dist_to_center(MoveInfo& info);
};

inline const MoveInfo& BoardConst::get_move_info(Move move) const
{
    LIBBOARDGAME_ASSERT(! move.is_null());
    LIBBOARDGAME_ASSERT(! move.is_pass());
    return m_move_info[move.to_int()];
}

inline const MovePoints& BoardConst::get_move_points(Move mv) const
{
    return get_move_info(mv).points;
}

inline const vector<Move>& BoardConst::get_moves(unsigned int piece,
                                                 Point p) const
{
    return m_moves_at[piece][p];
}

inline const vector<Move>& BoardConst::get_moves_diag(unsigned int piece,
                                                   Point p,
                                                   unsigned int diag_dir) const
{
    return m_moves_at_diag[diag_dir][piece][p];
}

inline const Piece& BoardConst::get_piece(unsigned int n) const
{
    LIBBOARDGAME_ASSERT(n < m_pieces.size());
    return m_pieces[n];
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_BOARD_CONST_H
