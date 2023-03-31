//-----------------------------------------------------------------------------
/** @file libpentobi_base/Board.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOARD_H
#define LIBPENTOBI_BASE_BOARD_H

#include "BoardConst.h"
#include "ColorMap.h"
#include "ColorMove.h"
#include "MoveList.h"
#include "PointList.h"
#include "PointState.h"
#include "Setup.h"
#include "StartingPoints.h"

namespace libpentobi_base {

class MoveMarker;

//-----------------------------------------------------------------------------

/** Blokus board.
    The implementation is speed-optimized for Monte-Carlo tree search. Only
    data that is needed during the MCTS search is computed incrementally.
    For the same reason, it does not provide an undo function, but instead
    a snapshot state that can can be restored quickly at the start of each
    MCTS simulation.

    @note The size of this class is large because it contains large members
    that are not allocated on the heap to avoid dereferencing pointers for
    speed reasons. It should be avoided to create instances of this class on
    the stack. */
class Board
{
public:
    using PointStateGrid = Grid<PointState>;

    /** Maximum number of pieces per player in any game variant. */
    static constexpr unsigned max_pieces = Setup::max_pieces;

    using PiecesLeftList = ArrayList<Piece, Piece::max_pieces>;

    static constexpr unsigned max_player_moves = max_pieces;

    /** Maximum number of moves in any game variant. */
    static constexpr unsigned max_moves = Color::range * max_player_moves;

    /** Use ANSI escape sequences for colored text output in operator>> */
    static inline bool color_output = false;


    explicit Board(Variant variant);

    /** Not implemented to avoid unintended copies.
        Use copy_from() to copy a board state. */
    Board(const Board&) = delete;

    /** Not implemented to avoid unintended copies.
        Use copy_from() to copy a board state. */
    Board& operator=(const Board&) = delete;

    Geometry::Iterator begin() const { return m_geo->begin(); }

    Geometry::Iterator end() const { return m_geo->end(); }

    Variant get_variant() const;

    Color::IntType get_nu_colors() const;

    Color::Range get_colors() const { return Color::Range(m_nu_colors); }

    unsigned get_nu_players() const;

    Piece::IntType get_nu_uniq_pieces() const;

    /** Number of instances of a unique piece per color. */
    unsigned get_nu_piece_instances(Piece piece) const;

    Color get_next(Color c) const;

    Color get_previous(Color c) const;

    const PieceTransforms& get_transforms() const;

    /** Get the state of an on-board point. */
    PointState get_point_state(Point p) const;

    const PointStateGrid& get_point_state() const;

    /** Get next color to play.
        The next color to play is the next color of the color of the last move
        played even if it has no more moves to play. */
    Color get_to_play() const;

    /** Get the player who plays the next move for the 4th color in
        Variant::classic_3. */
    Color::IntType get_alt_player() const;

    /** Equivalent to get_effective_to_play(get_to_play()) */
    Color get_effective_to_play() const;

    /** Get next color to play that still has moves.
        Colors are tried in their playing order starting with c. If no color
        has moves left, c is returned. */
    Color get_effective_to_play(Color c) const;

    const PiecesLeftList& get_pieces_left(Color c) const;

    bool is_piece_left(Color c, Piece piece) const;

    /** Check if no piece of a color has been placed on the board yet.
        This includes setup pieces and played moves. */
    bool is_first_piece(Color c) const;

    /** Get number of instances left of a piece.
        This value can be greater 1 in game variants that use multiple instances
        of a unique piece per player. */
    unsigned get_nu_left_piece(Color c, Piece piece) const;

    /** Get number of points of a color including the bonus. */
    ScoreType get_points(Color c) const { return m_state_color[c].points; }

    /** Get number of bonus points of a color. */
    ScoreType get_bonus(Color c) const;

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
        @pre ! mv.is_null()
        @pre get_nu_moves() < max_game_moves */
    void play(Color c, Move mv);

    /** More efficient version of play() if maximum piece size of current
        game variant is known at compile time. */
    template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
    void play(Color c, Move mv);

    /** Play a move.
        @pre ! mv.move.is_null()
        @pre get_nu_moves() < max_game_moves */
    void play(ColorMove mv);

    void set_to_play(Color c);

    void write(ostream& out, bool mark_last_move = true) const;

    /** Get the setup of the board before any moves were played.
        If the board was initialized without setup, the return value contains
        a setup with empty placement lists and Color(0) as the color to
        play. */
    const Setup& get_setup() const;

    bool has_setup() const;

    /** Get the total number of moves played by all colors.
        Does not include setup pieces.
        @see get_nu_onboard_pieces() */
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

    const ArrayList<ColorMove, max_moves>& get_moves() const;

    /** Generate all legal moves for a color.
        @param c The color
        @param marker A move marker reused for efficiency (needs to be clear)
        @param[out] moves The list of moves. */
    void gen_moves(Color c, MoveMarker& marker, MoveList& moves) const;

    bool has_moves(Color c) const;

    /** Check that no color has any moves left. */
    bool is_game_over() const;

    /** Check if a move is legal.
        @pre ! mv.is_null() */
    bool is_legal(Color c, Move mv) const;

    /** Check that point is not already occupied or adjacent to own color.
        Point::null() is an allowed argument and returns false. */
    bool is_forbidden(Point p, Color c) const;

    const GridExt<bool>& is_forbidden(Color c) const;

    /** Check that no points of move are already occupied or adjacent to own
        color.
        Does not check if the move is diagonally adjacent to an existing
        occupied point of the same color. */
    bool is_forbidden(Color c, Move mv) const;

    const BoardConst& get_board_const() const { return *m_bc; }

    BoardType get_board_type() const;

    PieceSet get_piece_set() const { return m_piece_set; }

    GeometryType get_geometry_type() const { return m_geometry_type; }

    bool is_callisto() const { return m_is_callisto; }

    /** Whether ties are broken in the current game variant. */
    bool get_break_ties() const { return m_is_callisto; }

    unsigned get_adj_status(Point p, Color c) const;

    /** Is a point in the center section that is forbidden for the 1-piece in
        Callisto?
        Always returns false for other game variants. */
    bool is_center_section(Point p) const { return m_is_center_section[p]; }

    /** Get score.
        The score is the number of points for a color minus the number of
        points of the opponent (or the average score of the opponents if there
        are more than two players). */
    ScoreType get_score(Color c) const;

    /** Specialized version of get_score().
        @pre get_nu_colors() == 2 */
    ScoreType get_score_twocolor(Color c) const;

    /** Specialized version of get_score().
        @pre get_nu_players() == 4 && get_nu_colors() == 4 */
    ScoreType get_score_multicolor(Color c) const;

    /** Specialized version of get_score().
        @pre get_nu_players() > 2 */
    ScoreType get_score_multiplayer(Color c) const;

    /** Specialized version of get_score().
        @pre get_nu_players() == 2 */
    ScoreType get_score_twoplayer(Color c) const;

    /** Get the place of a player in the game result.
        @param c The color of the player.
        @param[out] place The place of the player with that color. The place
        numbers start with 0. A place can be shared if several players have the
        same score. If a place is shared by n players, the following n-1 places
        are not used.
        @param[out] is_shared True if the place was shared. */
    void get_place(Color c, unsigned& place, bool& is_shared) const;

    const Geometry& get_geometry() const { return *m_geo; }

    /** See BoardConst::to_string() */
    string to_string(Move mv, bool with_piece_name = false) const;

    /** See BoardConst::from_string() */
    bool from_string(Move& mv, const string& s) const {
        return m_bc->from_string(mv, s); }

    bool find_move(const MovePoints& points, Piece piece, Move& mv) const;

    const Transform* find_transform(Move mv) const;

    const PieceInfo& get_piece_info(Piece piece) const;

    /** The 1x1 piece. */
    Piece get_one_piece() const { return m_one_piece; }

    Range<const Point> get_move_points(Move mv) const;

    Piece get_move_piece(Move mv) const;

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

    Move get_move_at(Point p) const;

    /** Remember the board state to quickly restore it later.
        A snapshot can only be restored from a position that was reached
        after playing moves from the snapshot position. */
    void take_snapshot();

    /** See take_snapshot() */
    void restore_snapshot();

private:
    /** Color-independent part of the board state. */
    struct StateBase
    {
        Color to_play;

        unsigned nu_onboard_pieces_all;

        PointStateGrid point_state;
    };

    /** Color-dependent part of the board state. */
    struct StateColor
    {
        GridExt<bool> forbidden;

        Grid<bool> is_attach_point;

        PiecesLeftList pieces_left;

        PieceMap<uint_fast8_t> nu_left_piece;

        unsigned nu_onboard_pieces;

        ScoreType points;
    };

    /** Snapshot for fast restoration of a previous position. */
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

    PieceSet m_piece_set;

    GeometryType m_geometry_type;

    Color::IntType m_nu_colors;

    bool m_is_callisto;

    unsigned m_nu_players;

    /** Caches m_bc->get_max_piece_size(). */
    unsigned m_max_piece_size;

    /** Caches m_bc->get_max_adj_attach(). */
    unsigned m_max_adj_attach;

    /** See get_needed_starting_points() */
    unsigned m_needed_starting_points;

    /** Bonus for playing all pieces. */
    ScoreType m_bonus_all_pieces;

    /** Bonus for playing the 1-piece last. */
    ScoreType m_bonus_one_piece;

    /** Caches get_piece_info(piece).get_score_points() */
    PieceMap<ScoreType> m_score_points;

    const BoardConst* m_bc;

    /** Caches m_bc->get_move_info_array() */
    BoardConst::MoveInfoArray m_move_info_array;

    /** Caches m_bc->get_move_info_ext_array() */
    BoardConst::MoveInfoExtArray m_move_info_ext_array;

    /** Caches m_bc->get_move_info_ext_2_array() */
    const MoveInfoExt2* m_move_info_ext_2_array;

    const Geometry* m_geo;

    /** See is_center_section(). */
    Grid<bool> m_is_center_section;

    /** The 1x1 piece. */
    Piece m_one_piece;

    ColorMap<PointList> m_attach_points;

    /** See get_second_color() */
    ColorMap<Color> m_second_color;

    ColorMap<char> m_color_char;

    ColorMap<const char*> m_color_esc_sequence;

    ColorMap<const char*> m_color_esc_sequence_text;

    ColorMap<const char*> m_color_name;

    ArrayList<ColorMove, max_moves> m_moves;

    Snapshot m_snapshot;

    Setup m_setup;

    StartingPoints m_starting_points;


    void gen_moves(Color c, Point p, Piece piece, unsigned adj_status,
                   MoveMarker& marker, MoveList& moves) const;

    bool has_moves(Color c, Point p) const;

    void init_variant(Variant variant);

    void optimize_attach_point_lists();

    template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
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


inline bool Board::find_move(const MovePoints& points, Piece piece,
                             Move& mv) const
{
    return m_bc->find_move(points, piece, mv);
}

inline unsigned Board::get_adj_status(Point p, Color c) const
{
    LIBBOARDGAME_ASSERT(m_bc->has_adj_status_points(p));
    auto i = m_bc->get_adj_status_points(p).begin();
    auto result = static_cast<unsigned>(is_forbidden(*i, c));
    for (unsigned j = 1; j < PrecompMoves::adj_status_nu_adj; ++j)
        result |= (static_cast<unsigned>(is_forbidden(*(++i), c)) << j);
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

inline BoardType Board::get_board_type() const
{
    return m_bc->get_board_type();
}

inline ColorMove Board::get_move(unsigned n) const
{
    return m_moves[n];
}

inline const MoveInfoExt2& Board::get_move_info_ext_2(Move mv) const
{
    LIBBOARDGAME_ASSERT(! mv.is_null());
    LIBBOARDGAME_ASSERT(mv.to_int() < m_bc->get_range());
    return *(m_move_info_ext_2_array + mv.to_int());
}

inline Piece Board::get_move_piece(Move mv) const
{
    return m_bc->get_move_piece(mv);
}

inline Range<const Point> Board::get_move_points(Move mv) const
{
    return m_bc->get_move_points(mv);
}

inline auto Board::get_moves() const -> const ArrayList<ColorMove, max_moves>&
{
    return m_moves;
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

inline unsigned Board::get_nu_piece_instances(Piece piece) const
{
    return m_bc->get_piece_info(piece).get_nu_instances();
}

inline Piece::IntType Board::get_nu_uniq_pieces() const
{
    return m_bc->get_nu_pieces();
}

inline const PieceInfo& Board::get_piece_info(Piece piece) const
{
    return m_bc->get_piece_info(piece);
}

inline const Board::PiecesLeftList& Board::get_pieces_left(Color c) const
{
    return m_state_color[c].pieces_left;
}

inline PointState Board::get_point_state(Point p) const
{
    return PointState(m_state_base.point_state[p].to_int());
}

inline const Board::PointStateGrid& Board::get_point_state() const
{
    return m_state_base.point_state;
}

inline Color Board::get_previous(Color c) const
{
    return c.get_previous(m_nu_colors);
}

inline ScoreType Board::get_score(Color c) const
{
    if (m_nu_colors == 2)
        return get_score_twocolor(c);
    if (m_nu_players == 2)
        return get_score_multicolor(c);
    return get_score_multiplayer(c);
}

inline ScoreType Board::get_score_twocolor(Color c) const
{
    LIBBOARDGAME_ASSERT(m_nu_colors == 2);
    auto points0 = get_points(Color(0));
    auto points1 = get_points(Color(1));
    if (c == Color(0))
        return points0 - points1;
    return points1 - points0;
}

inline ScoreType Board::get_score_twoplayer(Color c) const
{
    LIBBOARDGAME_ASSERT(m_nu_players == 2);
    if (m_nu_colors == 2)
        return get_score_twocolor(c);
    return get_score_multicolor(c);
}

inline ScoreType Board::get_score_multicolor(Color c) const
{
    LIBBOARDGAME_ASSERT(m_nu_players == 2 && m_nu_colors == 4);
    auto points0 = get_points(Color(0)) + get_points(Color(2));
    auto points1 = get_points(Color(1)) + get_points(Color(3));
    if (c == Color(0) || c == Color(2))
        return points0 - points1;
    return points1 - points0;
}

inline ScoreType Board::get_score_multiplayer(Color c) const
{
    LIBBOARDGAME_ASSERT(m_nu_players > 2);
    ScoreType score = 0;
    auto nu_players = static_cast<Color::IntType>(m_nu_players);
    for (Color i : get_colors())
        if (i != c)
            score -= get_points(i);
    score = get_points(c) + score / (static_cast<ScoreType>(nu_players) - 1);
    return score;
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
    return m_bc->get_transforms();
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

inline bool Board::is_first_piece(Color c) const
{
    return m_state_color[c].nu_onboard_pieces == 0;
}

inline bool Board::is_forbidden(Point p, Color c) const
{
    return m_state_color[c].forbidden[p];
}

inline const GridExt<bool>& Board::is_forbidden(Color c) const
{
    return m_state_color[c].forbidden;
}

inline bool Board::is_forbidden(Color c, Move mv) const
{
    auto points = get_move_points(mv);
    auto i = points.begin();
    auto end = points.end();
    do
        if (m_state_color[c].forbidden[*i])
            return true;
    while (++i != end);
    return false;
}

inline bool Board::is_piece_left(Color c, Piece piece) const
{
    LIBBOARDGAME_ASSERT(piece.to_int() < get_nu_uniq_pieces());
    return m_state_color[c].nu_left_piece[piece] > 0;
}

template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
inline void Board::place(Color c, Move mv)
{
    LIBBOARDGAME_ASSERT(m_max_piece_size == MAX_SIZE);
    LIBBOARDGAME_ASSERT(m_max_adj_attach == MAX_ADJ_ATTACH);
    auto& info = BoardConst::get_move_info<MAX_SIZE>(mv, m_move_info_array);
    auto& info_ext = BoardConst::get_move_info_ext<MAX_ADJ_ATTACH>(
                mv, m_move_info_ext_array);
    auto piece = info.get_piece();
    auto& state_color = m_state_color[c];
    LIBBOARDGAME_ASSERT(state_color.nu_left_piece[piece] > 0);
    auto score_points = m_score_points[piece];
    if (--state_color.nu_left_piece[piece] == 0)
    {
        state_color.pieces_left.remove_fast(piece);
        if (MAX_SIZE == 22) // GembloQ
        {
            LIBBOARDGAME_ASSERT(m_bonus_all_pieces == 0);
            LIBBOARDGAME_ASSERT(m_bonus_one_piece == 0);
        }
        else if (state_color.pieces_left.empty())
        {
            state_color.points += m_bonus_all_pieces;
            if (MAX_SIZE == 7) // Nexos
                LIBBOARDGAME_ASSERT(m_bonus_one_piece == 0);
            else if (score_points == 1)
                state_color.points += m_bonus_one_piece;
        }
    }
    ++m_state_base.nu_onboard_pieces_all;
    ++state_color.nu_onboard_pieces;
    state_color.points += score_points;
    auto i = info.begin();
    auto end = info.end();
    do
    {
        m_state_base.point_state[*i] = PointState(c);
        for_each_color([&](Color c) {
            m_state_color[c].forbidden[*i] = true;
        });
    }
    while (++i != end);
    if (MAX_SIZE == 7) // Nexos
    {
        LIBBOARDGAME_ASSERT(info_ext.size_adj_points == 0);
        i = info_ext.begin_attach();
        end = i + info_ext.size_attach_points;
    }
    else
    {
        end = info_ext.end_adj();
        for (i = info_ext.begin_adj(); i != end; ++i)
            state_color.forbidden[*i] = true;
        LIBBOARDGAME_ASSERT(i == info_ext.begin_attach());
        end += info_ext.size_attach_points;
    }
    auto& attach_points = m_attach_points[c];
    auto n = attach_points.size();
    do
        if (! state_color.forbidden[*i] && ! state_color.is_attach_point[*i])
        {
            state_color.is_attach_point[*i] = true;
            attach_points.get_unchecked(n) = *i;
            ++n;
        }
    while (++i != end);
    attach_points.resize(n);
}

template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
inline void Board::play(Color c, Move mv)
{
    place<MAX_SIZE, MAX_ADJ_ATTACH>(c, mv);
    m_moves.push_back(ColorMove(c, mv));
    m_state_base.to_play = get_next(c);
}

inline void Board::play(ColorMove mv)
{
    play(mv.color, mv.move);
}

inline void Board::restore_snapshot()
{
    LIBBOARDGAME_ASSERT(m_snapshot.moves_size <= m_moves.size());
    auto& geo = get_geometry();
    m_moves.resize(m_snapshot.moves_size);
    m_state_base.to_play = m_snapshot.state_base.to_play;
    m_state_base.nu_onboard_pieces_all =
        m_snapshot.state_base.nu_onboard_pieces_all;
    m_state_base.point_state.memcpy_from(m_snapshot.state_base.point_state,
                                         geo);
    for (Color c : get_colors())
    {
        const auto& snapshot_state = m_snapshot.state_color[c];
        auto& state = m_state_color[c];
        state.forbidden.copy_from(snapshot_state.forbidden, geo);
        state.is_attach_point.copy_from(snapshot_state.is_attach_point, geo);
        state.pieces_left = snapshot_state.pieces_left;
        state.nu_left_piece = snapshot_state.nu_left_piece;
        state.nu_onboard_pieces = snapshot_state.nu_onboard_pieces;
        state.points = snapshot_state.points;
        m_attach_points[c].resize(m_snapshot.attach_points_size[c]);
    }
}

inline void Board::set_to_play(Color c)
{
    m_state_base.to_play = c;
}

inline string Board::to_string(Move mv, bool with_piece_name) const
{
    return m_bc->to_string(mv, with_piece_name);
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
