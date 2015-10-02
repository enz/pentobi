//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardConst.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOARD_CONST_H
#define LIBPENTOBI_BASE_BOARD_CONST_H

#include <array>
#include <algorithm>
#include "Geometry.h"
#include "Marker.h"
#include "MoveInfo.h"
#include "PieceInfo.h"
#include "PieceTransforms.h"
#include "PrecompMoves.h"
#include "Variant.h"
#include "libpentobi_base/Color.h"
#include "libpentobi_base/ColorMap.h"
#include "libboardgame_util/ArrayList.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_util::ArrayList;

//-----------------------------------------------------------------------------

/** Constant precomputed data that is shared between all instances of Board
    with a given board type and set of unique pieces per color. */
class BoardConst
{
public:
    /** See get_adj_status_list() */
    typedef
    ArrayList<Point, PrecompMoves::adj_status_nu_adj, unsigned short>
    AdjStatusList;

    /** Maximum number of unique pieces per color. */
    static const unsigned max_pieces = 22;

    static const unsigned max_moves_at_point = 40;

    /** Get the single instance for a given board size.
        The instance is created the first time this function is called.
        This function is not thread-safe. */
    static const BoardConst& get(Variant variant);

    Piece::IntType get_nu_pieces() const;

    unsigned get_total_piece_points() const;

    unsigned get_max_piece_size() const;

    const PieceInfo& get_piece_info(Piece piece) const;

    unsigned get_nu_attach_points(Piece piece) const;

    bool get_piece_by_name(const string& name, Piece& piece) const;

    const PieceTransforms& get_transforms() const;

    /** Get move info.
        @pre move.is_regular() */
    const MoveInfo& get_move_info(Move mv) const;

    /** Get pointer to move info array.
        Can be used to speed up the access to the move info by avoiding the
        multiple pointer dereferencing of Board::get_move_info(Move) */
    const MoveInfo* get_move_info_array() const;

    /** Get pointer to extended move info array.
        Can be used to speed up the access to the move info by avoiding the
        multiple pointer dereferencing of Board::get_move_info_ext(Move) */
    const MoveInfoExt* get_move_info_ext_array() const;

    /** Get extended move info.
        @pre move.is_regular() */
    const MoveInfoExt& get_move_info_ext(Move mv) const;

    const MoveInfoExt2& get_move_info_ext_2(Move mv) const;

    const MoveInfoExt2* get_move_info_ext_2_array() const;

    unsigned get_nu_moves() const;

    bool find_move(const MovePoints& points, Move& move) const;

    bool find_move(const MovePoints& points, Piece piece, Move& move) const;

    /** Get all moves of a piece at a point constrained by the forbidden
        status of adjacent points. */
    PrecompMoves::Range get_moves(Piece piece, Point p,
                                  unsigned adj_status = 0) const
    {
        return m_precomp_moves.get_moves(piece, p, adj_status);
    }

    const PrecompMoves& get_precomp_moves() const { return m_precomp_moves; }

    BoardType get_board_type() const;

    const Geometry& get_geometry() const;

    /** List containing the points used for the adjacent status.
        Contains the first PrecompMoves::adj_status_nu_adj points of
        Geometry::get_adj() concatenated with Geometry::get_diag(). */
    const AdjStatusList& get_adj_status_list(Point p) const
    {
        return m_adj_status_list[p];
    }

    /** Convert a move to its string representation.
        The string representation is a comma-separated list of points (without
        spaces between the commas or points). If with_piece_name is true,
        it is prepended by the piece name in square brackets (also without any
        spaces). The representation without the piece name is used by the SGF
        files and GTP interface used by Pentobi (version >= 0.2). */
    string to_string(Move mv, bool with_piece_name = false) const;

    Move from_string(const string& s) const;

    /** Sort move points using the ordering used in blksgf files. */
    void sort(MovePoints& points) const;

private:
    typedef ArrayList<Move, max_moves_at_point> LocalMovesList;

    /** See m_full_move_table */
    typedef array<PieceMap<Grid<LocalMovesList>>, PrecompMoves::nu_adj_status>
        FullMoveTable;

    /** Local variable used during construction.
        Making this variable static slightly speeds up construction and
        a thread-safe construction is not needed. */
    static Marker s_marker;

    /** Local variable used during construction.
        See s_marker why this variable is static. */
    static Grid<array<ArrayList<Point, PrecompMoves::adj_status_nu_adj>,
                     PrecompMoves::nu_adj_status>>
        s_adj_status;

    Piece::IntType m_nu_pieces;

    unsigned m_total_piece_points;

    unsigned m_max_piece_size;

    unsigned m_nu_moves;

    BoardType m_board_type;

    const Geometry& m_geo;

    vector<PieceInfo> m_pieces;

    Grid<AdjStatusList> m_adj_status_list;

    unique_ptr<PieceTransforms> m_transforms;

    PieceMap<unsigned> m_nu_attach_points{0};

    unique_ptr<MoveInfo[]> m_move_info;

    unique_ptr<MoveInfoExt[]> m_move_info_ext;

    unique_ptr<MoveInfoExt2[]> m_move_info_ext_2;

    /** Non-compact representation of lists of moves of a piece at a point
        constrained by the forbidden status of adjacent points.
        Only used during construction of m_moves_range and m_move_lists. */
    unique_ptr<FullMoveTable> m_full_move_table;

    PrecompMoves m_precomp_moves;

    /** Value for comparing points using the ordering used in blksgf files.
        As specified in doc/blksgf/Pentobi-SGF.html, the order should be
        (a1, b1, ..., a2, b2, ...) with y going upwards whereas the convention
        for Point is that y goes downwards. */
    Grid<unsigned> m_compare_val;

    BoardConst(BoardType board_type, PieceSet piece_set);

    void create_move(unsigned& moves_created, Piece piece,
                     const MovePoints& points, Point label_pos);

    void create_moves();

    void create_moves(unsigned& moves_created, Piece piece);

    void init_adj_status();

    void init_adj_status(
                       Point p,
                       array<bool, PrecompMoves::adj_status_nu_adj>& forbidden,
                       unsigned i);

    void init_symmetry_info();
};

inline BoardType BoardConst::get_board_type() const
{
    return m_board_type;
}

inline const Geometry& BoardConst::get_geometry() const
{
    return m_geo;
}

inline unsigned BoardConst::get_max_piece_size() const
{
    return m_max_piece_size;
}

inline const MoveInfo& BoardConst::get_move_info(Move mv) const
{
    LIBBOARDGAME_ASSERT(mv.to_int() < m_nu_moves);
    return m_move_info[mv.to_int()];
}

inline const MoveInfo* BoardConst::get_move_info_array() const
{
    return m_move_info.get();
}

inline const MoveInfoExt& BoardConst::get_move_info_ext(Move mv) const
{
    LIBBOARDGAME_ASSERT(mv.to_int() < m_nu_moves);
    return m_move_info_ext[mv.to_int()];
}

inline const MoveInfoExt2& BoardConst::get_move_info_ext_2(Move mv) const
{
    LIBBOARDGAME_ASSERT(mv.to_int() < m_nu_moves);
    return m_move_info_ext_2[mv.to_int()];
}

inline const MoveInfoExt* BoardConst::get_move_info_ext_array() const
{
    return m_move_info_ext.get();
}

inline const MoveInfoExt2* BoardConst::get_move_info_ext_2_array() const
{
    return m_move_info_ext_2.get();
}

inline unsigned BoardConst::get_nu_moves() const
{
    return m_nu_moves;
}

inline unsigned BoardConst::get_nu_attach_points(Piece piece) const
{
    return m_nu_attach_points[piece];
}

inline Piece::IntType BoardConst::get_nu_pieces() const
{
    return m_nu_pieces;
}

inline const PieceInfo& BoardConst::get_piece_info(Piece piece) const
{
    LIBBOARDGAME_ASSERT(piece.to_int() < m_pieces.size());
    return m_pieces[piece.to_int()];
}

inline unsigned BoardConst::get_total_piece_points() const
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
