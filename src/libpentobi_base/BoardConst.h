//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardConst.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOARD_CONST_H
#define LIBPENTOBI_BASE_BOARD_CONST_H

#include <array>
#include "Geometry.h"
#include "Marker.h"
#include "MoveInfo.h"
#include "PieceInfo.h"
#include "PieceTransforms.h"
#include "PrecompMoves.h"
#include "Variant.h"
#include "libboardgame_util/ArrayList.h"
#include "libboardgame_util/Range.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_util::ArrayList;
using libboardgame_util::Range;

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

    /** Start of the MoveInfo array, which can be cached by the user in
        performance-critical code and then passed into the static version of
        get_move_info(). */
    typedef const void* MoveInfoArray;

    /** Start of the MoveInfoExt array, which can be cached by the user in
        performance-critical code and then passed into the static version of
        get_move_info_ext(). */
    typedef const void* MoveInfoExtArray;

    /** Get the single instance for a given board size.
        The instance is created the first time this function is called.
        This function is not thread-safe. */
    static const BoardConst& get(Variant variant);

    template<unsigned MAX_SIZE>
    static const MoveInfo<MAX_SIZE>&
    get_move_info(Move mv, MoveInfoArray move_info_array);

    template<unsigned MAX_ADJ_ATTACH>
    static const MoveInfoExt<MAX_ADJ_ATTACH>&
    get_move_info_ext(Move mv, MoveInfoExtArray move_info_ext_array);


    Piece::IntType get_nu_pieces() const;

    const PieceInfo& get_piece_info(Piece piece) const;

    unsigned get_nu_attach_points(Piece piece) const;

    bool get_piece_by_name(const string& name, Piece& piece) const;

    const PieceTransforms& get_transforms() const;

    unsigned get_max_piece_size() const { return m_max_piece_size; }

    unsigned get_max_adj_attach() const { return m_max_adj_attach; }

    Range<const Point> get_move_points(Move mv) const;

    /** Return start of move points array.
        For unrolling loops, there are guaranteed to be as many elements
        as the maximum piece size in the current game variant. If the piece
        is smaller, the remaining points are guaranteed to be Point::null(). */
    const Point* get_move_points_begin(Move mv) const;

    template<unsigned MAX_SIZE>
    const Point* get_move_points_begin(Move mv) const;

    Piece get_move_piece(Move mv) const;

    template<unsigned MAX_SIZE>
    Piece get_move_piece(Move mv) const;

    MoveInfoArray get_move_info_array() const { return m_move_info.get(); }

    /** Get pointer to extended move info array.
        Can be used to speed up the access to the move info by avoiding the
        multiple pointer dereferencing of Board::get_move_info_ext(Move) */
    MoveInfoExtArray get_move_info_ext_array() const;

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

    BoardType get_board_type() const { return m_board_type; };

    PieceSet get_piece_set() const { return m_piece_set; }

    const Geometry& get_geometry() const;

    /** List containing the points used for the adjacent status.
        Contains the first PrecompMoves::adj_status_nu_adj points of
        Geometry::get_adj() concatenated with Geometry::get_diag().
        Elements above end() may be accessed and contain Point::null()
        for easy unrolling of loops. */
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
    struct MallocFree
    {
        void operator()(void* x) { free(x); }
    };


    Piece::IntType m_nu_pieces;

    unsigned m_nu_moves;

    unsigned m_max_piece_size;

    /** See MoveInfoExt */
    unsigned m_max_adj_attach;

    BoardType m_board_type;

    PieceSet m_piece_set;

    const Geometry& m_geo;

    vector<PieceInfo> m_pieces;

    Grid<AdjStatusList> m_adj_status_list;

    unique_ptr<PieceTransforms> m_transforms;

    PieceMap<unsigned> m_nu_attach_points{0};

    /** Array of MoveInfo<MAX_SIZE> with MAX_SIZE being the maximum piece size
        in the corresponding game variant.
        See comments at MoveInfo. */
    unique_ptr<void, MallocFree> m_move_info;

    /** Array of MoveInfoExt<MAX_ADJ_ATTACH> with MAX_ADJ_ATTACH being the
        maximum total number of attach points and adjacent points of a piece in
        the corresponding game variant.
        See comments at MoveInfoExt. */
    unique_ptr<void, MallocFree> m_move_info_ext;

    unique_ptr<MoveInfoExt2[]> m_move_info_ext_2;

    PrecompMoves m_precomp_moves;

    /** Value for comparing points using the ordering used in blksgf files.
        As specified in doc/blksgf/Pentobi-SGF.html, the order should be
        (a1, b1, ..., a2, b2, ...) with y going upwards whereas the convention
        for Point is that y goes downwards. */
    Grid<unsigned> m_compare_val;


    BoardConst(BoardType board_type, PieceSet piece_set);

    template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
    void create_move(unsigned& moves_created, Piece piece,
                     const MovePoints& points, Point label_pos);

    void create_moves();

    template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
    void create_moves(unsigned& moves_created, Piece piece);

    template<unsigned MAX_SIZE>
    const MoveInfo<MAX_SIZE>& get_move_info(Move mv) const;

    void init_adj_status();

    void init_adj_status(
                       Point p,
                       array<bool, PrecompMoves::adj_status_nu_adj>& forbidden,
                       unsigned i);

    void init_symmetry_info();
};

inline const Geometry& BoardConst::get_geometry() const
{
    return m_geo;
}

template<unsigned MAX_SIZE>
inline const MoveInfo<MAX_SIZE>&
BoardConst::get_move_info(Move mv, MoveInfoArray move_info_array)
{
    LIBBOARDGAME_ASSERT(! mv.is_null());
    return *(static_cast<const MoveInfo<MAX_SIZE>*>(move_info_array)
             + mv.to_int());
}

template<unsigned MAX_SIZE>
inline const MoveInfo<MAX_SIZE>& BoardConst::get_move_info(Move mv) const
{
    LIBBOARDGAME_ASSERT(m_max_piece_size == MAX_SIZE);
    return get_move_info<MAX_SIZE>(mv, m_move_info.get());
}

template<unsigned MAX_ADJ_ATTACH>
inline const MoveInfoExt<MAX_ADJ_ATTACH>&
BoardConst::get_move_info_ext(Move mv, MoveInfoExtArray move_info_ext_array)
{
    LIBBOARDGAME_ASSERT(! mv.is_null());
    return *(static_cast<const MoveInfoExt<MAX_ADJ_ATTACH>*>(
                 move_info_ext_array) + mv.to_int());
}

inline const MoveInfoExt2& BoardConst::get_move_info_ext_2(Move mv) const
{
    LIBBOARDGAME_ASSERT(mv.to_int() < m_nu_moves);
    return m_move_info_ext_2[mv.to_int()];
}

inline auto BoardConst::get_move_info_ext_array() const -> MoveInfoExtArray
{
    return m_move_info_ext.get();
}

inline const MoveInfoExt2* BoardConst::get_move_info_ext_2_array() const
{
    return m_move_info_ext_2.get();
}

template<unsigned MAX_SIZE>
inline Piece BoardConst::get_move_piece(Move mv) const
{
    return get_move_info<MAX_SIZE>(mv).get_piece();
}

inline Piece BoardConst::get_move_piece(Move mv) const
{
    if (m_max_piece_size == 5)
        return get_move_piece<5>(mv);
    else if (m_max_piece_size == 6)
        return get_move_piece<6>(mv);
    else
    {
        LIBBOARDGAME_ASSERT(m_max_piece_size == 7);
        return get_move_piece<7>(mv);
    }
}

inline Range<const Point> BoardConst::get_move_points(Move mv) const
{
    if (m_max_piece_size == 5)
    {
        auto& info = get_move_info<5>(mv);
        return Range<const Point>(info.begin(), info.end());
    }
    else if (m_max_piece_size == 6)
    {
        auto& info = get_move_info<6>(mv);
        return Range<const Point>(info.begin(), info.end());
    }
    else
    {
        LIBBOARDGAME_ASSERT(m_max_piece_size == 7);
        auto& info = get_move_info<7>(mv);
        return Range<const Point>(info.begin(), info.end());
    }
}

inline const Point* BoardConst::get_move_points_begin(Move mv) const
{
    if (m_max_piece_size == 5)
        return get_move_points_begin<5>(mv);
    else if (m_max_piece_size == 6)
        return get_move_points_begin<6>(mv);
    else
    {
        LIBBOARDGAME_ASSERT(m_max_piece_size == 7);
        return get_move_points_begin<7>(mv);
    }
}

template<unsigned MAX_SIZE>
inline const Point* BoardConst::get_move_points_begin(Move mv) const
{
    return get_move_info<MAX_SIZE>(mv).begin();
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

inline const PieceTransforms& BoardConst::get_transforms() const
{
    return *m_transforms;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_BOARD_CONST_H
