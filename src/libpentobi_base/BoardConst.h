//-----------------------------------------------------------------------------
/** @file BoardConst.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOARD_CONST_H
#define LIBPENTOBI_BASE_BOARD_CONST_H

#include <array>
#include <algorithm>
#include "Geometry.h"
#include "Grid.h"
#include "Marker.h"
#include "Move.h"
#include "MoveInfo.h"
#include "Point.h"
#include "Piece.h"
#include "PieceTransforms.h"
#include "libpentobi_base/Color.h"
#include "libpentobi_base/ColorMap.h"
#include "libboardgame_util/ArrayList.h"

namespace libpentobi_base {

using libboardgame_util::ArrayList;

//-----------------------------------------------------------------------------

enum BoardType
{
    board_type_classic,

    board_type_duo,

    board_type_trigon
};

//-----------------------------------------------------------------------------

/** Constant precomputed data that is shared between all instances of Board. */
class BoardConst
{
public:
    static const unsigned int max_pieces = 22;

    /** Get the single instance for a given board size.
        The instance is created the first time this function is called. */
    static const BoardConst& get(BoardType board_type);

    /** Index of a given forbidden status of adjacent points for a color.
        @param s0 If the point at Direction::get_enum_adj(0) is forbidden
        @param s1 If the point at Direction::get_enum_adj(1) is forbidden
        @param s2 If the point at Direction::get_enum_adj(2) is forbidden
        @param s3 If the point at Direction::get_enum_adj(3) is forbidden */
    static unsigned int get_adj_status_index(bool s0, bool s1, bool s2,
                                             bool s3);

    unsigned int get_nu_pieces() const;

    unsigned int get_total_piece_points() const;

    const Piece& get_piece(unsigned int n) const;

    bool get_piece_index_by_name(const string& name,
                                 unsigned int& index) const;

    const PieceTransforms& get_transforms() const;

    const MoveInfo& get_move_info(Move move) const;

    const MovePoints& get_move_points(Move mv) const;

    bool find_move(const MovePoints& points, Move& move) const;

    /** Get all moves of a piece at a point constrained by the forbidden
        status of adjacent points. */
    const vector<Move>& get_moves(unsigned int piece, Point p,
                                  unsigned int adj_status_index = 0) const;

    BoardType get_board_type() const;

    const Geometry& get_geometry() const;

private:
    unsigned int m_nu_pieces;

    unsigned int m_total_piece_points;

    BoardType m_board_type;

    const Geometry& m_geometry;

    vector<Piece> m_pieces;

    unique_ptr<PieceTransforms> m_transforms;

    vector<MoveInfo> m_move_info;

    /** Moves of a piece at a point constrained by the forbidden status of
        adjacent points. */
    array<array<Grid<vector<Move>>, max_pieces>, 16> m_moves;

    /** Local variable reused for efficiency. */
    Marker m_marker;

    /** Directions of forbidden neighbors for a given adjacent status index.
        The directions are represented by integers as in
        Direction::get_enum_adj() */
    array<ArrayList<unsigned int, 4>, 16> m_adj_status;

    BoardConst(BoardType board_type);

    void create_move(unsigned int piece, const Piece::Points& coord_points,
                     CoordPoint center);

    void create_moves(unsigned int piece);

    void init_symmetry_info();

    bool is_compatible_with_adj_status(Point p, unsigned int adj_status_index,
                                       const MovePoints& points) const;

    void set_adj_and_corner_points(MoveInfo& info);
};

inline unsigned int BoardConst::get_adj_status_index(bool s0, bool s1, bool s2,
                                                     bool s3)
{
    unsigned int i = 0;
    if (s0)
        i |= (1 << 0);
    if (s1)
        i |= (1 << 1);
    if (s2)
        i |= (1 << 2);
    if (s3)
        i |= (1 << 3);
    return i;
}

inline BoardType BoardConst::get_board_type() const
{
    return m_board_type;
}

inline const Geometry& BoardConst::get_geometry() const
{
    return m_geometry;
}

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
                                            Point p,
                                            unsigned int adj_status_index) const
{
    return m_moves[adj_status_index][piece][p];
}

inline unsigned int BoardConst::get_nu_pieces() const
{
    return m_nu_pieces;
}

inline const Piece& BoardConst::get_piece(unsigned int n) const
{
    LIBBOARDGAME_ASSERT(n < m_pieces.size());
    return m_pieces[n];
}

inline unsigned int BoardConst::get_total_piece_points() const
{
    return m_total_piece_points;
}

inline const PieceTransforms& BoardConst::get_transforms() const
{
    return *m_transforms;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_BOARD_CONST_H
