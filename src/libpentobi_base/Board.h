//-----------------------------------------------------------------------------
/** @file libpentobi_base/Board.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOARD_H
#define LIBPENTOBI_BASE_BOARD_H

#include <cstddef>
#include <memory>
#include "BoardConst.h"
#include "ColorMap.h"
#include "ColorMove.h"
#include "Variant.h"
#include "Geometry.h"
#include "Grid.h"
#include "MoveMarker.h"
#include "PointList.h"
#include "PointState.h"
#include "Setup.h"
#include "StartingPoints.h"
#include "libboardgame_util/Unused.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

/** Blokus board.
    @note @ref libboardgame_avoid_stack_allocation */
class Board
{
public:
    typedef Grid<PointState> PointStateGrid;

    typedef PrecompMoves::LocalMovesListRange LocalMovesListRange;

    /** Iterator over all points on the board. */
    class Iterator
        : public GeometryIterator
    {
        friend class Board;

    public:
        Iterator(const Board& bd);
    };

    /** Maximum number of pieces per player in any game variant. */
    static const unsigned max_pieces = Setup::max_pieces;

    typedef ArrayList<Piece, BoardConst::max_pieces> PiecesLeftList;

    static const unsigned max_player_moves = max_pieces;

    /** Maximum number of moves in any game variant. */
    static const unsigned max_game_moves = Color::range * max_player_moves;

    /** Use ANSI escape sequences for colored text output in operator>> */
    static bool color_output;

    Board(Variant variant);

    /** Not implemented to avoid unintended copies.
        Use copy_from() to copy a board state. */
    Board(const Board&) = delete;

    /** Not implemented to avoid unintended copies.
        Use copy_from() to copy a board state. */
    Board& operator=(const Board&) = delete;

    Variant get_variant() const;

    Color::IntType get_nu_colors() const;

    /** Number of colors that are not played alternately.
        This is equal to get_nu_colors() apart from Variant::classic_3. */
    Color::IntType get_nu_nonalt_colors() const;

    unsigned get_nu_players() const;


    Piece::IntType get_nu_uniq_pieces() const;

    /** Get number of pieces per player in the current game variant. */
    unsigned get_nu_pieces() const;

    /** Number of instances of each unique piece per color. */
    unsigned get_nu_piece_instances() const;

    unsigned get_max_player_moves() const;

    /** Maximum number of non-pass moves in the current game variant. */
    unsigned get_max_game_moves() const;

    Color get_next(Color c) const;

    Color get_previous(Color c) const;

    const PieceTransforms& get_transforms() const;

    /** Get the state of an on-board point. */
    PointState get_point_state(Point p) const;

    bool is_empty(Point p) const;

    const PointStateGrid& get_grid() const;

    /** Get next color to play.
        The next color to play is the next color of the color of the last move
        played even if it has no more moves to play (apart from a pass
        move). */
    Color get_to_play() const;

    /** Get the player who plays the next move for the 4th color in
        Variant::classic_3. */
    Color::IntType get_alt_player() const;

    /** Get next color to play that still has moves.
        Colors are tried in their playing order starting with get_to_play().
        If no color has moves left, get_to_play() is returned. */
    Color get_effective_to_play() const;

    const PiecesLeftList& get_pieces_left(Color c) const;

    bool is_piece_left(Color c, Piece piece) const;

    /** Check if no piece of a color has been placed on the board yet.
        This includes setup pieces and played moves. */
    bool is_first_piece(Color c) const;

    /** Get number of instances left of a piece.
        This value can be greater 1 in game variants that use multiple instances
        of a unique piece per player. */
    unsigned get_nu_left_piece(Color c, Piece piece) const;

    /** Get number of points by a color including the bonus. */
    unsigned get_points(Color c) const;

    /** Is a point a potential attachment point for a color.
        Does not check if the point is forbidden. */
    bool is_attach_point(Point p, Color c) const;

    /** Get potential attachment points for a color.
        Does not check if the point is forbidden. */
    const PointList& get_attach_points(Color c) const;

    /** Initialize the current board for a given game variant.
        @param variant The game variant
        @param setup An optional setup position to initialize the board
        with. */
    void init(Variant variant, const Setup* setup = nullptr);

    /** Clear the current board without changing the current game variant.
        See init(Variant,const Setup*) */
    void init(const Setup* setup = nullptr);

    /** Copy the board state and move history from another board.
        This is like an assignment operator but because boards are rarely
        copied by value and copying is expensive, it is an explicit function to
        avoid accidental copying. */
    void copy_from(const Board& bd);

    /** Play a move.
        @pre mv.is_regular()
        @pre get_nu_moves() < max_game_moves */
    void play(Color c, Move mv);

    /** Play a move for the current color to play.
        @pre mv.is_regular()
        @pre get_nu_moves() < max_game_moves */
    void play(Move mv);

    /** Play a move.
        @pre mv.move.is_regular()
        @pre get_nu_moves() < max_game_moves */
    void play(ColorMove mv);

    void undo();

    void set_to_play(Color c);

    void write(ostream& out, bool mark_last_move = true) const;

    /** Get the setup of the board before any moves were played.
        If the board was initialized without setup, the return value contains
        a setup with empty placement lists and Color(0) as the color to
        play. */
    const Setup& get_setup() const;

    bool has_setup() const;

    unsigned get_nu_moves() const;

    /** Get the number of pieces on board.
        This is the number of setup pieces, if the board was initialized
        with a setup position, plus the number of pieces played as moves. */
    unsigned get_nu_onboard_pieces() const;

    /** Get the number of pieces on board of a color.
        This is the number of setup pieces, if the board was initialized
        with a setup position, plus the number of pieces played as moves. */
    unsigned get_nu_onboard_pieces(Color c) const;

    ColorMove get_move(unsigned n) const;

    /** Generate all moves for one player.
        The generated moves do not include the pass move. */
    void gen_moves(Color c, ArrayList<Move, Move::range>& moves) const;

    void gen_moves(Color c, Point p,
                   ArrayList<Move, Move::range>& moves) const;

    void gen_moves(Color c, Point p, MoveMarker& marker,
                   ArrayList<Move, Move::range>& moves) const;

    bool has_moves(Color c) const;

    /** Check that no color has any moves left. */
    bool is_game_over() const;

    /** Check if a move is legal.
        @pre mv.is_regular() */
    bool is_legal(Color c, Move mv) const;

    /** Check if a move is legal for the current color to play.
        @pre mv.is_regular() */
    bool is_legal(Move mv) const;

    /** Check that point is not already occupied or adjacent to own color.
        @param c
        @param p The point. Off-board points are allowed and return true. */
    bool is_forbidden(Point p, Color c) const;

    const Grid<bool>& is_forbidden(Color c) const;

    /** Check that no points of move are already occupied or adjacent to own
        color.
        Does not check if the move is diagonally adjacent to an existing
        occupied point of the same color. */
    bool is_forbidden(Color c, Move mv) const;

    const BoardConst& get_board_const() const;

    BoardType get_board_type() const;

    unsigned get_adj_status(Point p, Color c) const;

    LocalMovesListRange get_moves(Piece piece, Point p,
                                  unsigned adj_status) const;

    /** Get score.
        The score is the number of points for a color minus the number of
        points of the opponent (or the average score of the opponents if there
        are more than two players). */
    int get_score(Color c) const;

    /** Get the place of a player in the game result.
        @param c The color of the player.
        @param[out] place The place of the player with that color. The place
        numbers start with 0. A place can be shared if several players have the
        same score. If a place is shared by n players, the following n-1 places
        are not used.
        @param[out] is_shared True if the place was shared. */
    void get_place(Color c, unsigned& place, bool& is_shared) const;

    const Geometry& get_geometry() const;

    /** See BoardConst::to_string() */
    string to_string(Move mv, bool with_piece_name = true) const;

    /** See BoardConst::from_string() */
    Move from_string(const string& s) const;

    bool find_move(const MovePoints& points, Move& move) const;

    const PieceInfo& get_piece_info(Piece piece) const;

    bool get_piece_by_name(const string& name, Piece& piece) const;

    const MoveInfo& get_move_info(Move mv) const;

    const MoveInfoExt& get_move_info_ext(Move mv) const;

    const MoveInfoExt2& get_move_info_ext_2(Move mv) const;

    bool is_colored_starting_point(Point p) const;

    bool is_colorless_starting_point(Point p) const;

    Color get_starting_point_color(Point p) const;

    const ArrayList<Point,StartingPoints::max_starting_points>&
                                            get_starting_points(Color c) const;

    /** Get the second color in game variants in which a player plays two
        colors.
        @return The second color of the player that plays color c, or c if
        the player plays only one color in the current game variant or
        if the game variant is classic_3. */
    Color get_second_color(Color c) const;

    bool is_same_player(Color c1, Color c2) const;

    Move get_move_at(Point p) const;

    /** Remember the board state to quickly restore it later.
        A snapshot can only be restored from a position that was reached
        after playing moves from the snapshot position. */
    void take_snapshot();

    /** See take_snapshot() */
    void restore_snapshot();

private:
    /** Color-independent part of the board state for fast snapshot
        restoration.
        Must have only POD-like members such that it can quickly be copied
        with memcpy in copy_from(). */
    struct StateBase
    {
        Color to_play;

        unsigned nu_onboard_pieces_all;

        PointStateGrid point_state;
    };

    /** Color-dependent part of the board state for fast snapshot restoration.
        Must have only POD-like members such that it can quickly be copied
        with memcpy in copy_from(). */
    struct StateColor
    {
        Grid<bool> forbidden;

        Grid<bool> is_attach_point;

        /** @note Order dependency: This must be the first of all remaining
            members because they are copied with memcpy in take_snapshot()
            and restore_snapshot() */
        PiecesLeftList pieces_left;

        PieceMap<uint_fast8_t> nu_left_piece;

        unsigned nu_onboard_pieces;

        unsigned points;
    };

    /** Snapshot for fast restoration of a previous position.
        @note state_base and state_color are restored with a single memcpy,
        they must be subsequent members and their order must not be changed. */
    struct Snapshot
    {
        StateBase state_base;

        ColorMap<StateColor> state_color;

        unsigned moves_size;

        ColorMap<unsigned> attach_points_size;
    };

    StateBase m_state_base;

    ColorMap<StateColor> m_state_color;

    Variant m_variant;

    Color::IntType m_nu_colors;

    unsigned m_nu_players;

    const BoardConst* m_board_const;

    /** Bonus for playing all pieces. */
    unsigned m_bonus_all_pieces;

    /** Bonus for playing the 1-piece last. */
    unsigned m_bonus_one_piece;

    /** See get_nu_piece_instances() */
    uint_fast8_t m_nu_piece_instances;

    /** Same as m_board_const->get_move_info_array() */
    const MoveInfo* m_move_info_array;

    /** Same as m_board_const->get_move_info_ext_array() */
    const MoveInfoExt* m_move_info_ext_array;

    /** Same as m_board_const->get_move_info_ext_2_array() */
    const MoveInfoExt2* m_move_info_ext_2_array;

    const Geometry* m_geo;

    ColorMap<PointList> m_attach_points;

    /** See get_second_color() */
    ColorMap<Color> m_second_color;

    ColorMap<char> m_color_char;

    ColorMap<const char*> m_color_esc_sequence;

    ColorMap<const char*> m_color_esc_sequence_text;

    ColorMap<const char*> m_color_name;

    ArrayList<ColorMove, max_game_moves> m_moves;

    unique_ptr<Snapshot> m_snapshot;

    Setup m_setup;

    StartingPoints m_starting_points;

    /** Local variable during move generation.
        Reused for efficiency. */
    mutable MoveMarker m_marker;

    void gen_moves(Color c, Point p, unsigned adj_status, MoveMarker& marker,
                   ArrayList<Move,Move::range>& moves) const;

    bool has_moves(Color c, Point p) const;

    void init_variant(Variant variant);

    void optimize_attach_point_lists();

    void place(Color c, Move mv);

    void place_setup(const Setup& setup);

    void write_pieces_left(ostream& out, Color c,
                           const PiecesLeftList& pieces_left, unsigned begin,
                           unsigned end) const;

    void write_color_info_line1(ostream& out, Color c) const;

    void write_color_info_line2(ostream& out, Color c,
                                const PiecesLeftList& pieces_left) const;

    void write_color_info_line3(ostream& out, Color c,
                                const PiecesLeftList& pieces_left) const;

    void write_info_line(ostream& out, unsigned y,
                         const ColorMap<PiecesLeftList>& pieces_left) const;
};

typedef Board::Iterator BoardIterator;

inline Board::Iterator::Iterator(const Board& bd)
    : GeometryIterator(*bd.m_geo)
{
}

inline bool Board::find_move(const MovePoints& points, Move& move) const
{
    return m_board_const->find_move(points, move);
}

inline Move Board::from_string(const string& s) const
{
    return m_board_const->from_string(s);
}

inline unsigned Board::get_adj_status(Point p, Color c) const
{
    auto& adj_status_list = m_board_const->get_adj_status_list(p);
    auto i = adj_status_list.begin();
    auto end = adj_status_list.end();
    LIBBOARDGAME_ASSERT(i != end);
    unsigned val = 1;
    unsigned result = is_forbidden(*i, c) ? val : 0;
    for (unsigned j = 1; j < PrecompMoves::adj_status_nu_adj; ++j)
    {
        if (++i == end)
            break;
        val <<= 1;
        if (is_forbidden(*i, c))
            result |= val;
    }
    return result;
}

inline Color::IntType Board::get_alt_player() const
{
    LIBBOARDGAME_ASSERT(m_variant == Variant::classic_3);
    return static_cast<Color::IntType>(get_nu_onboard_pieces(Color(3)) % 3);
}

inline const PointList&  Board::get_attach_points(Color c) const
{
    return m_attach_points[c];
}

inline const BoardConst& Board::get_board_const() const
{
    return *m_board_const;
}

inline BoardType Board::get_board_type() const
{
    return m_board_const->get_board_type();
}

inline const Geometry& Board::get_geometry() const
{
    return *m_geo;
}

inline const Board::PointStateGrid& Board::get_grid() const
{
    return m_state_base.point_state;
}

inline unsigned Board::get_max_game_moves() const
{
    return m_nu_colors * get_max_player_moves();
}

inline unsigned Board::get_max_player_moves() const
{
    return get_nu_pieces();
}

inline ColorMove Board::get_move(unsigned n) const
{
    return m_moves[n];
}

inline const MoveInfo& Board::get_move_info(Move mv) const
{
    LIBBOARDGAME_ASSERT(! mv.is_null());
    LIBBOARDGAME_ASSERT(mv.to_int() < m_board_const->get_nu_all_moves());
    return *(m_move_info_array + mv.to_int());
}

inline const MoveInfoExt& Board::get_move_info_ext(Move mv) const
{
    LIBBOARDGAME_ASSERT(! mv.is_null());
    LIBBOARDGAME_ASSERT(mv.to_int() < m_board_const->get_nu_all_moves());
    return *(m_move_info_ext_array + mv.to_int());
}

inline const MoveInfoExt2& Board::get_move_info_ext_2(Move mv) const
{
    LIBBOARDGAME_ASSERT(! mv.is_null());
    LIBBOARDGAME_ASSERT(mv.to_int() < m_board_const->get_nu_all_moves());
    return *(m_move_info_ext_2_array + mv.to_int());
}

inline Board::LocalMovesListRange Board::get_moves(Piece piece, Point p,
                                                   unsigned adj_status) const
{
    return m_board_const->get_moves(piece, p, adj_status);
}

inline Color Board::get_next(Color c) const
{
    return c.get_next(m_nu_colors);
}

inline Color::IntType Board::get_nu_colors() const
{
    return m_nu_colors;
}

inline unsigned Board::get_nu_left_piece(Color c, Piece piece) const
{
    LIBBOARDGAME_ASSERT(piece.to_int() < get_nu_uniq_pieces());
    return m_state_color[c].nu_left_piece[piece];
}

inline unsigned Board::get_nu_moves() const
{
    return m_moves.size();
}

inline Color::IntType Board::get_nu_nonalt_colors() const
{
    return m_variant != Variant::classic_3 ? m_nu_colors : 3;
}

inline unsigned Board::get_nu_onboard_pieces() const
{
    return m_state_base.nu_onboard_pieces_all;
}

inline unsigned Board::get_nu_onboard_pieces(Color c) const
{
    return m_state_color[c].nu_onboard_pieces;
}

inline unsigned Board::get_nu_players() const
{
    return m_nu_players;
}

inline unsigned Board::get_nu_piece_instances() const
{
    return m_nu_piece_instances;
}

inline unsigned Board::get_nu_pieces() const
{
    return m_nu_piece_instances * m_board_const->get_nu_pieces();
}

inline Piece::IntType Board::get_nu_uniq_pieces() const
{
    return m_board_const->get_nu_pieces();
}

inline const PieceInfo& Board::get_piece_info(Piece piece) const
{
    return m_board_const->get_piece_info(piece);
}

inline bool Board::get_piece_by_name(const string& name, Piece& piece) const
{
    return m_board_const->get_piece_by_name(name, piece);
}

inline const Board::PiecesLeftList& Board::get_pieces_left(Color c) const
{
    return m_state_color[c].pieces_left;
}

inline PointState Board::get_point_state(Point p) const
{
    return PointState(m_state_base.point_state[p].to_int());
}

inline unsigned Board::get_points(Color c) const
{
    return m_state_color[c].points;
}

inline Color Board::get_previous(Color c) const
{
    return c.get_previous(m_nu_colors);
}

inline int Board::get_score(Color c) const
{
    if (m_nu_colors == 2)
    {
        LIBBOARDGAME_ASSERT(m_nu_players == 2);
        unsigned points0 = get_points(Color(0));
        unsigned points1 = get_points(Color(1));
        if (c == Color(0))
            return points0 - points1;
        else
            return points1 - points0;
    }
    else if (m_nu_players == 2)
    {
        LIBBOARDGAME_ASSERT(m_nu_colors == 4);
        unsigned points0 = get_points(Color(0)) + get_points(Color(2));
        unsigned points1 = get_points(Color(1)) + get_points(Color(3));
        if (c == Color(0) || c == Color(2))
            return points0 - points1;
        else
            return points1 - points0;
    }
    else
    {
        int score = 0;
        auto nu_players = static_cast<Color::IntType>(m_nu_players);
        for (ColorIterator i(nu_players); i; ++i)
            if (*i != c)
                score -= get_points(*i);
        score = get_points(c) + score / (static_cast<int>(nu_players) - 1);
        return score;
    }
}

inline Color Board::get_second_color(Color c) const
{
    return m_second_color[c];
}

inline const Setup& Board::get_setup() const
{
    return m_setup;
}

inline Color Board::get_starting_point_color(Point p) const
{
    return m_starting_points.get_starting_point_color(p);
}

inline const ArrayList<Point,StartingPoints::max_starting_points>&
                                       Board::get_starting_points(Color c) const
{
    return m_starting_points.get_starting_points(c);
}

inline Color Board::get_to_play() const
{
    return m_state_base.to_play;
}

inline const PieceTransforms& Board::get_transforms() const
{
    return m_board_const->get_transforms();
}

inline Variant Board::get_variant() const
{
    return m_variant;
}

inline void Board::init(const Setup* setup)
{
    init(m_variant, setup);
}

inline bool Board::is_attach_point(Point p, Color c) const
{
    return m_state_color[c].is_attach_point[p];
}

inline bool Board::is_colored_starting_point(Point p) const
{
    return m_starting_points.is_colored_starting_point(p);
}

inline bool Board::is_colorless_starting_point(Point p) const
{
    return m_starting_points.is_colorless_starting_point(p);
}

inline bool Board::is_empty(Point p) const
{
    return get_point_state(p).is_empty();
}

inline bool Board::is_first_piece(Color c) const
{
    return m_state_color[c].nu_onboard_pieces == 0;
}

inline bool Board::is_forbidden(Point p, Color c) const
{
    return m_state_color[c].forbidden[p];
}

inline const Grid<bool>& Board::is_forbidden(Color c) const
{
    return m_state_color[c].forbidden;
}

inline bool Board::is_forbidden(Color c, Move mv) const
{
    auto& info = get_move_info(mv);
    auto i = info.begin();
    auto end = info.end();
    do
        if (m_state_color[c].forbidden[*i])
            return true;
    while (++i != end);
    return false;
}

inline bool Board::is_legal(Move mv) const
{
    return is_legal(m_state_base.to_play, mv);
}

inline bool Board::is_legal(Color c, Move mv) const
{
    auto& info = get_move_info(mv);
    if (! is_piece_left(c, info.get_piece()))
        return false;
    bool has_attach_point = false;
    auto i = info.begin();
    auto end = info.end();
    do
    {
        if (m_state_color[c].forbidden[*i])
            return false;
        if (is_attach_point(*i, c))
            has_attach_point = true;
    }
    while (++i != end);
    if (has_attach_point)
        return true;
    if (! is_first_piece(c))
        return false;
    i = info.begin();
    do
        if (is_colorless_starting_point(*i)
            || (is_colored_starting_point(*i)
                && get_starting_point_color(*i) == c))
            return true;
    while (++i != end);
    return false;
}

inline bool Board::is_piece_left(Color c, Piece piece) const
{
    LIBBOARDGAME_ASSERT(piece.to_int() < get_nu_uniq_pieces());
    return m_state_color[c].nu_left_piece[piece] > 0;
}

inline bool Board::is_same_player(Color c1, Color c2) const
{
    return (c1 == c2 || c1 == m_second_color[c2]);
}

inline void Board::place(Color c, Move mv)
{
    auto& info = get_move_info(mv);
    auto& info_ext = get_move_info_ext(mv);
    auto piece = info.get_piece();
    auto piece_size = info.size();
    auto& state_color = m_state_color[c];
    LIBBOARDGAME_ASSERT(state_color.nu_left_piece[piece] > 0);
    if (--state_color.nu_left_piece[piece] == 0)
    {
        state_color.pieces_left.remove_fast(piece);
        if (state_color.pieces_left.empty())
        {
            state_color.points += m_bonus_all_pieces;
            if (piece_size == 1)
                state_color.points += m_bonus_one_piece;
        }
    }
    ++m_state_base.nu_onboard_pieces_all;
    ++state_color.nu_onboard_pieces;
    state_color.points += piece_size;
    auto i = info.begin();
    auto end = i + piece_size;
    do
    {
        m_state_base.point_state[*i] = c;
        for_each_color([&](Color c) {
            m_state_color[c].forbidden[*i] = true;
        });
    }
    while (++i != end);
    i = info_ext.begin_attach();
    end = info_ext.end_attach();
    do
        if (! state_color.forbidden[*i] && ! state_color.is_attach_point[*i])
        {
            state_color.is_attach_point[*i] = true;
            m_attach_points[c].push_back(*i);
        }
    while (++i != end);
    LIBBOARDGAME_ASSERT(i == info_ext.begin_adj());
    end += info_ext.size_adj_points;
    do
        state_color.forbidden[*i] = true;
    while (++i != end);
}

inline void Board::play(Color c, Move mv)
{
    place(c, mv);
    m_moves.push_back(ColorMove(c, mv));
    m_state_base.to_play = get_next(c);
}

inline void Board::play(Move mv)
{
    play(m_state_base.to_play, mv);
}

inline void Board::play(ColorMove mv)
{
    play(mv.color, mv.move);
}

inline void Board::restore_snapshot()
{
    LIBBOARDGAME_ASSERT(m_snapshot);
    LIBBOARDGAME_ASSERT(m_snapshot->moves_size <= m_moves.size());
    m_moves.resize(m_snapshot->moves_size);
    m_state_base.to_play = m_snapshot->state_base.to_play;
    m_state_base.nu_onboard_pieces_all =
        m_snapshot->state_base.nu_onboard_pieces_all;
    m_state_base.point_state.memcpy_from(m_snapshot->state_base.point_state,
                                         *m_geo);
    for (ColorIterator i(m_nu_colors); i; ++i)
    {
        const auto& snapshot_state = m_snapshot->state_color[*i];
        auto& state = m_state_color[*i];
        state.forbidden.memcpy_from(snapshot_state.forbidden, *m_geo);
        state.is_attach_point.memcpy_from(snapshot_state.is_attach_point,
                                          *m_geo);
        // Uncomment once is_trivially_copyable is implemented in GCC and MSVC
        //static_assert(is_trivially_copyable<StateColor>::value, "")
        memcpy(&state.pieces_left, &snapshot_state.pieces_left,
               sizeof(StateColor) - offsetof(StateColor, pieces_left));
        LIBBOARDGAME_ASSERT(m_snapshot->attach_points_size[*i]
                            <= m_attach_points[*i].size());
        m_attach_points[*i].resize(m_snapshot->attach_points_size[*i]);
    }
}

inline void Board::set_to_play(Color c)
{
    m_state_base.to_play = c;
}

inline string Board::to_string(Move mv, bool with_piece_name) const
{
    return m_board_const->to_string(mv, with_piece_name);
}

//-----------------------------------------------------------------------------

inline ostream& operator<<(ostream& out, const Board& bd)
{
    bd.write(out);
    return out;
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_BOARD_H
