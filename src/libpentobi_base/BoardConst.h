//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardConst.h
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOARD_CONST_H
#define LIBPENTOBI_BASE_BOARD_CONST_H

#include <array>
#include <algorithm>
#include "Variant.h"
#include "Geometry.h"
#include "Grid.h"
#include "Marker.h"
#include "Move.h"
#include "MoveInfo.h"
#include "Point.h"
#include "PieceInfo.h"
#include "PieceMap.h"
#include "PieceTransforms.h"
#include "libpentobi_base/Color.h"
#include "libpentobi_base/ColorMap.h"
#include "libboardgame_util/ArrayList.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_util::ArrayList;

//-----------------------------------------------------------------------------

enum class BoardType
{
    classic,

    duo,

    trigon,

    trigon_3
};

//-----------------------------------------------------------------------------

/** Constant precomputed data that is shared between all instances of Board
    with a given board type and set of unique pieces per color. */
class BoardConst
{
public:
    /** Maximum number of unique pieces per color. */
    static const unsigned max_pieces = 22;

    static const unsigned max_moves_at_point = 40;

    /** The maximum sum of the sizes of all precomputed move lists in any
        game variant. */
    static const unsigned max_move_lists_sum_length = 1425934;

    /** Begin/end range for lists with local moves.
        See get_moves(). */
    class LocalMovesListRange
    {
    public:
        LocalMovesListRange(const Move* begin, const Move* end)
        {
            m_begin = begin;
            m_end = end;
        }

        const Move* begin() const
        {
            return m_begin;
        }

        const Move* end() const
        {
            return m_end;
        }

    private:
        const Move* m_begin;

        const Move* m_end;
    };

    /** Compressed begin/end range for lists with local moves.
        This struct is used in the private implementation and will be unpacked
        into a LocalMovesListRange as a return value for get_moves().
        The struct is public to make it reusable for implementing similar
        precomputed move lists outside this class. */
    struct ListIndex
    {
        unsigned begin : 24;

        unsigned size : 8;

        ListIndex()
        {
        }

        ListIndex(unsigned begin, unsigned size)
        {
            LIBBOARDGAME_ASSERT(begin < max_move_lists_sum_length);
            LIBBOARDGAME_ASSERT(begin + size <= max_move_lists_sum_length);
            LIBBOARDGAME_ASSERT(begin < (1 << 24));
            this->begin = begin & ((1 << 24) - 1);
            LIBBOARDGAME_ASSERT(size < (1 << 8));
            this->size = size & ((1 << 8) - 1);
        }
    };

    /** The number of neighbors used for computing the adjacent status.
        The adjacent status is a single number that encodes the forbidden
        status of the first adj_status_nu_adj neighbors (from the list
        Geometry::get_adj_diag()). It is used for speeding up the matching of
        moves at a given point. Increasing this number will make the
        precomputed lists shorter but exponentially increase the number of
        lists and the total memory used for all lists. Therefore, the optimal
        value for speeding up the matching depends on the CPU cache size. */
    static const unsigned adj_status_nu_adj = 5;

    static const unsigned nu_adj_status = 1 << adj_status_nu_adj;

    /** Get the single instance for a given board size.
        The instance is created the first time this function is called. */
    static const BoardConst& get(Variant variant);

    Piece::IntType get_nu_pieces() const;

    unsigned get_total_piece_points() const;

    unsigned get_max_piece_size() const;

    const PieceInfo& get_piece_info(Piece piece) const;

    bool get_piece_by_name(const string& name, Piece& piece) const;

    const PieceTransforms& get_transforms() const;

    /** Get move info.
        @pre move.is_regular() */
    const MoveInfo& get_move_info(Move move) const;

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
    const MoveInfoExt& get_move_info_ext(Move move) const;

    const MoveInfoExt2& get_move_info_ext_2(Move mv) const;

    const MoveInfoExt2* get_move_info_ext_2_array() const;

    unsigned get_nu_all_moves() const;

    bool find_move(const MovePoints& points, Move& move) const;

    /** Get all moves of a piece at a point constrained by the forbidden
        status of adjacent points. */
    LocalMovesListRange get_moves(Piece piece, Point p,
                                  unsigned adj_status = 0) const;

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
    typedef ArrayList<Move,max_moves_at_point> LocalMovesList;

    /** See m_full_move_table */
    typedef array<PieceMap<Grid<LocalMovesList>>,nu_adj_status> FullMoveTable;

    Piece::IntType m_nu_pieces;

    unsigned m_total_piece_points;

    unsigned m_max_piece_size;

    BoardType m_board_type;

    const Geometry& m_geo;

    vector<PieceInfo> m_pieces;

    unique_ptr<PieceTransforms> m_transforms;

    vector<MoveInfo> m_move_info;

    vector<MoveInfoExt> m_move_info_ext;

    vector<MoveInfoExt2> m_move_info_ext_2;

    /** Non-compact representation of lists of moves of a piece at a point
        constrained by the forbidden status of adjacent points.
        Only used during construction of m_moves_range and m_move_lists. */
    unique_ptr<FullMoveTable> m_full_move_table;

    /** See m_move_lists. */
    Grid<array<PieceMap<ListIndex>,nu_adj_status>> m_moves_range;

    /** Compact representation of lists of moves of a piece at a point
        constrained by the forbidden status of adjacent points.
        All lists are stored in a single array; m_moves_range contains
        information about the actual begin/end indices. */
    unique_ptr<Move[]> m_move_lists;

    /** Sum of sizes of all lists in m_full_move_table.
        Only used during construction of m_moves_range and m_move_lists. */
    size_t m_move_lists_sum_length;

    /** Local variable reused for efficiency. */
    Marker m_marker;

    /** Forbidden neighbors for a given adjacent status index at a given point.
        Only used during construction. */
    Grid<array<ArrayList<Point,adj_status_nu_adj>,nu_adj_status>> m_adj_status;

    BoardConst(BoardType board_type, Variant variant);

    void create_move(Piece piece, const PiecePoints& coord_points,
                     Point center);

    void create_moves();

    void create_moves(Piece piece);

    void init_adj_status();

    void init_adj_status(Point p, array<bool, adj_status_nu_adj>& forbidden,
                         unsigned i);

    void init_symmetry_info();

    bool is_compatible_with_adj_status(Point p, unsigned adj_status,
                                       const MoveInfo& info) const;

    void reserve_info(size_t nu_moves);

    void set_adj_and_attach_points(const MoveInfo& info, MoveInfoExt& info_ext);
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

inline const MoveInfo& BoardConst::get_move_info(Move move) const
{
    LIBBOARDGAME_ASSERT(move.to_int() < m_move_info.size());
    return m_move_info[move.to_int()];
}

inline const MoveInfo* BoardConst::get_move_info_array() const
{
    return &m_move_info.front();
}

inline const MoveInfoExt& BoardConst::get_move_info_ext(Move move) const
{
    LIBBOARDGAME_ASSERT(move.to_int() < m_move_info_ext.size());
    return m_move_info_ext[move.to_int()];
}

inline const MoveInfoExt2& BoardConst::get_move_info_ext_2(Move mv) const
{
    LIBBOARDGAME_ASSERT(mv.to_int() < m_move_info_ext_2.size());
    return m_move_info_ext_2[mv.to_int()];
}

inline const MoveInfoExt* BoardConst::get_move_info_ext_array() const
{
    return &m_move_info_ext.front();
}

inline const MoveInfoExt2* BoardConst::get_move_info_ext_2_array() const
{
    return &m_move_info_ext_2.front();
}

inline BoardConst::LocalMovesListRange BoardConst::get_moves(
                               Piece piece, Point p, unsigned adj_status) const
{
    ListIndex idx = m_moves_range[p][adj_status][piece];
    auto begin = m_move_lists.get() + idx.begin;
    auto end = begin + idx.size;
    return LocalMovesListRange(begin, end);
}

inline unsigned BoardConst::get_nu_all_moves() const
{
    return static_cast<unsigned>(m_move_info.size());
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
