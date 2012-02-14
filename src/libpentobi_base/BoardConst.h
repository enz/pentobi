//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardConst.h */
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

    board_type_trigon,

    board_type_trigon_3
};

//-----------------------------------------------------------------------------

/** Constant precomputed data that is shared between all instances of Board. */
class BoardConst
{
public:
    static const unsigned int max_pieces = 22;

    static const unsigned int max_moves_at_point = 40;

    typedef ArrayList<Move,max_moves_at_point> LocalMovesList;

    /** The number of neighbors used for computing the adjacent status.
        The adjacent status is a single number that encodes the forbidden
        status of the first adj_status_nu_adj neighbors (from the list
        Geometry::get_adj_diag()). It is used for speeding up the matching of
        moves at a given point. */
    static const unsigned int adj_status_nu_adj = 4;

    static const unsigned int nu_adj_status_index = 1 << adj_status_nu_adj;

    /** Get the single instance for a given board size.
        The instance is created the first time this function is called. */
    static const BoardConst& get(BoardType board_type);

    unsigned int get_nu_pieces() const;

    unsigned int get_total_piece_points() const;

    const Piece& get_piece(unsigned int n) const;

    bool get_piece_index_by_name(const string& name,
                                 unsigned int& index) const;

    const PieceTransforms& get_transforms() const;

    const MoveInfo& get_move_info(Move move) const;

    const MoveInfoExt& get_move_info_ext(Move move) const;

    const MovePoints& get_move_points(Move mv) const;

    bool find_move(const MovePoints& points, Move& move) const;

    /** Get all moves of a piece at a point constrained by the forbidden
        status of adjacent points. */
    const LocalMovesList& get_moves(unsigned int piece, Point p,
                                    unsigned int adj_status_index = 0) const;

    BoardType get_board_type() const;

    const Geometry& get_geometry() const;

    /** Convert a move to its string representation.
        The string representation is a comma-separated list of points (without
        spaces between the commas or points). If with_piece_name is true,
        it is prepended by the piece name in square brackets (also without any
        spaces). The representation without the piece name is used by the SGF
        files and GTP interface used by Pentobi (version >= 0.2). */
    string to_string(Move mv, bool with_piece_name) const;

    Move from_string(const string& s) const;

private:
    unsigned int m_nu_pieces;

    unsigned int m_total_piece_points;

    BoardType m_board_type;

    const Geometry& m_geometry;

    vector<Piece> m_pieces;

    unique_ptr<PieceTransforms> m_transforms;

    vector<MoveInfo> m_move_info;

    vector<MoveInfoExt> m_move_info_ext;

    /** Moves of a piece at a point constrained by the forbidden status of
        adjacent points. */
    array<array<Grid<LocalMovesList>,max_pieces>,nu_adj_status_index> m_moves;

    /** Local variable reused for efficiency. */
    Marker m_marker;

    /** Forbidden neighbors for a given adjacent status index at a given point.
        Only used during construction. */
    Grid<array<ArrayList<Point, adj_status_nu_adj>, nu_adj_status_index>>
                                                                   m_adj_status;

    BoardConst(BoardType board_type);

    void create_move(unsigned int piece_index,
                     const Piece::Points& coord_points, Point center);

    void create_moves(unsigned int piece_index);

    void init_adj_status();

    void init_adj_status(Point p, array<bool, adj_status_nu_adj>& forbidden,
                         unsigned int i);

    void init_symmetry_info();

    bool is_compatible_with_adj_status(Point p, unsigned int adj_status_index,
                                       const MovePoints& points) const;

    void set_adj_and_attach_points(MoveInfo& info);
};

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

inline const MoveInfoExt& BoardConst::get_move_info_ext(Move move) const
{
    LIBBOARDGAME_ASSERT(! move.is_null());
    LIBBOARDGAME_ASSERT(! move.is_pass());
    return m_move_info_ext[move.to_int()];
}

inline const MovePoints& BoardConst::get_move_points(Move mv) const
{
    return get_move_info(mv).points;
}

inline const ArrayList<Move,40>& BoardConst::get_moves(unsigned int piece,
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
