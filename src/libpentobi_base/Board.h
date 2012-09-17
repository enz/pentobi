//-----------------------------------------------------------------------------
/** @file libpentobi_base/Board.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOARD_H
#define LIBPENTOBI_BASE_BOARD_H

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

using libboardgame_util::NullTermList;

//-----------------------------------------------------------------------------

/** Blokus board.
    @note @ref libboardgame_avoid_stack_allocation */
class Board
{
public:
    typedef Grid<PointState> PointStateGrid;

    typedef BoardConst::LocalMovesListRange LocalMovesListRange;

    /** Iterator over all points on the board. */
    class Iterator
        : public GeometryIterator
    {
        friend class Board;

    public:
        Iterator(const Board& bd);
    };

    /** Maximum number of pieces per player in any game variant. */
    static const unsigned int max_pieces = Setup::max_pieces;

    typedef ArrayList<Piece,BoardConst::max_pieces> PiecesLeftList;

    /** Maximum number of moves for a player in a game.
        Non-alternating moves and alternating pass moves are allowed but the
        game must end after all colors have passed in a row. Therefore, the
        maximum number of moves is reached in case that a piece move is
        followed by (Color::range-1) pass moves and an extra Color::range pass
        moves at the end. */
    static const unsigned int max_player_moves = Color::range * max_pieces + 1;

    static const unsigned int max_nonpass_player_moves = max_pieces;

    /** Maximum number of moves in any game variant.
        Includes an extra pass move per color at the end of the game. */
    static const unsigned int max_game_moves = Color::range * max_player_moves;

    static const unsigned int max_nonpass_game_moves =
        Color::range * max_nonpass_player_moves;

    /** Use ANSI escape sequences for colored text output in operator>> */
    static bool color_output;

    Board(Variant variant);

    Variant get_variant() const;

    unsigned int get_nu_colors() const;

    Color get_next(Color c) const;

    Color get_previous(Color c) const;

    unsigned int get_nu_pieces() const;

    const PieceTransforms& get_transforms() const;

    /** Get the state of an on-board point. */
    PointState get_point_state(Point p) const;

    bool is_empty(Point p) const;

    bool is_onboard(Point p) const;

    const PointStateGrid& get_grid() const;

    /** Get next color to play.
        The next color to play is the next color of the color of the last move
        played even if it has no more moves to play (apart from a pass
        move). */
    Color get_to_play() const;

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
    unsigned int get_nu_left_piece(Color c, Piece piece) const;

    unsigned int get_points(Color c) const;

    unsigned int get_bonus(Color c) const;

    unsigned int get_points_with_bonus(Color c) const;

    unsigned int get_points_left(Color c) const;

    Move get_played_move(Point p) const;

    /** Is a point a potential attachment point for a color.
        Does not check if the point is forbidden.. */
    bool is_attach_point(Point p, Color c) const;

    /** Get potential attachment points for a color.
        Does not check if the point is forbidden.. */
    const PointList& get_attach_points(Color c) const;

    /** Initialize the current board for a given game variant.
        @param variant The game variant
        @param setup An optional setup position to initialize the board
        with. */
    void init(Variant variant, const Setup* setup = 0);

    /** Clear the current board without changing the current game variant.
        See init(Variant,const Setup*) */
    void init(const Setup* setup = 0);

    /** Copy the board state and move history from another board.
        This is like an assignment operator but because boards are rarely copied
        by value and copying is expensive, it is an explicit function to avoid
        accidental copying. */
    void copy_from(const Board& bd);

    /** Play a move.
        @pre get_nu_moves() < max_game_moves */
    void play(Color c, Move move);

    /** Play a pass move.
        @pre get_nu_moves() < max_game_moves */
    void play_pass(Color c);

    /** Play a move that is known to be not a pass move.
        Slightly faster than play(Color,Move)
        @pre get_nu_moves() < max_game_moves */
    void play_nonpass(Color c, Move move);

    /** See play(Color,Move) */
    void play(ColorMove move);

    /** Calls play(Color,Move) with the current color to play. */
    void play(Move mv);

    void undo();

    void set_to_play(Color c);

    void write(ostream& out, bool mark_last_move = true) const;

    /** Get the setup of the board before any moves were played.
        If the board was initialized without setup, the return value contains
        a setup with empty placement lists and Color(0) as the color to
        play. */
    const Setup& get_setup() const;

    bool has_setup() const;

    unsigned int get_nu_moves() const;

    /** Get the number of pieces on board.
        This is the number of setup pieces, if the board was initialized
        with a setup position, plus the number of pieces played as moves. */
    unsigned int get_nu_onboard_pieces() const;

    /** Get the number of pieces on board of a color.
        This is the number of setup pieces, if the board was initialized
        with a setup position, plus the number of pieces played as moves. */
    unsigned int get_nu_onboard_pieces(Color c) const;

    ColorMove get_move(unsigned int n) const;

    /** Generate all moves for one player.
        The generated moves do not include the pass move. */
    void gen_moves(Color c, ArrayList<Move, Move::range>& moves) const;

    void gen_moves(Color c, Point p, MoveMarker& marker,
                   ArrayList<Move, Move::range>& moves) const;

    bool has_moves(Color c) const;

    /** Check that no color has any moves left. */
    bool is_game_over() const;

    bool is_legal(Color c, Move mv) const;

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

    unsigned int get_adj_status_index(Point p, Color c) const;

    LocalMovesListRange get_moves(Piece piece, Point p,
                                  unsigned int adj_status_index) const;

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
    void get_place(Color c, unsigned int& place, bool& is_shared) const;

    const Geometry& get_geometry() const;

    /** See BoardConst::to_string() */
    string to_string(Move mv, bool with_piece_name = true) const;

    /** See BoardConst::from_string() */
    Move from_string(const string& s) const;

    bool find_move(const MovePoints& points, Move& move) const;

    const PieceInfo& get_piece_info(Piece piece) const;

    bool get_piece_by_name(const string& name, Piece& piece) const;

    const MovePoints& get_move_points(Move mv) const;

    const MoveInfo& get_move_info(Move move) const;

    const MoveInfoExt& get_move_info_ext(Move move) const;

    bool is_colored_starting_point(Point p) const;

    bool is_colorless_starting_point(Point p) const;

    Color get_starting_point_color(Point p) const;

    const ArrayList<Point,StartingPoints::max_starting_points>&
                                             get_starting_points(Color c) const;

    /** Get the second color in game variants in which a player plays two
        colors.
        @return The second color of the player that plays color c, or c if
        the player plays only one color in the current game variant. */
    Color get_second_color(Color c) const;

    bool is_same_player(Color c1, Color c2) const;

private:
    Variant m_variant;

    unsigned int m_nu_colors;

    const BoardConst* m_board_const;

    const Geometry* m_geometry;

    Color m_to_play;

    PointStateGrid m_point_state;

    ColorMap<Grid<bool>> m_forbidden;

    ColorMap<Grid<bool>> m_is_attach_point;

    ColorMap<PointList> m_attach_points;

    Grid<Move> m_played_move;

    ColorMap<PiecesLeftList> m_pieces_left;

    ColorMap<PieceMap<unsigned int>> m_nu_left_piece;

    /** See get_second_color() */
    ColorMap<Color> m_second_color;

    Setup m_setup;

    ArrayList<ColorMove, max_game_moves> m_moves;

    ColorMap<unsigned int> m_nu_onboard_pieces;

    unsigned int m_nu_onboard_pieces_all;

    ColorMap<char> m_color_char;

    ColorMap<const char*> m_color_esc_sequence;

    ColorMap<const char*> m_color_esc_sequence_text;

    ColorMap<const char*> m_color_name;

    StartingPoints m_starting_points;

    /** Local variable during move generation.
        Reused for efficiency. */
    mutable MoveMarker m_marker;

    void gen_moves(Color c, Point p, unsigned int adj_status_index,
                   MoveMarker& marker,
                   ArrayList<Move, Move::range>& moves) const;

    bool has_moves(Color c, Point p) const;

    void init_variant(Variant variant);

    void place(Color c, Move mv);

    void write_pieces_left(ostream& out, Color c, unsigned int begin,
                           unsigned int end) const;

    void write_color_info_line1(ostream& out, Color c) const;

    void write_color_info_line2(ostream& out, Color c) const;

    void write_color_info_line3(ostream& out, Color c) const;

    void write_info_line(ostream& out, unsigned int y) const;
};

typedef Board::Iterator BoardIterator;

inline Board::Iterator::Iterator(const Board& bd)
    : GeometryIterator(*bd.m_geometry)
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

inline unsigned int Board::get_adj_status_index(Point p, Color c) const
{
    unsigned int result = 0;
    unsigned int n = 0;
    for (NullTermList<Point, 12>::Iterator i(m_geometry->get_adj_diag(p));
         n < BoardConst::adj_status_nu_adj && i; ++i, ++n)
        if (is_forbidden(*i, c))
            result |= (1 << n);
    return result;
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
    return *m_geometry;
}

inline const Board::PointStateGrid& Board::get_grid() const
{
    return m_point_state;
}

inline ColorMove Board::get_move(unsigned int n) const
{
    return m_moves[n];
}

inline const MoveInfo& Board::get_move_info(Move move) const
{
    return m_board_const->get_move_info(move);
}

inline const MoveInfoExt& Board::get_move_info_ext(Move move) const
{
    return m_board_const->get_move_info_ext(move);
}

inline Board::LocalMovesListRange Board::get_moves(Piece piece, Point p,
                                           unsigned int adj_status_index) const
{
    return m_board_const->get_moves(piece, p, adj_status_index);
}

inline Color Board::get_next(Color c) const
{
    return c.get_next(m_nu_colors);
}

inline unsigned int Board::get_nu_colors() const
{
    return m_nu_colors;
}

inline unsigned int Board::get_nu_left_piece(Color c, Piece piece) const
{
    LIBBOARDGAME_ASSERT(piece.to_int() < get_nu_pieces());
    return m_nu_left_piece[c][piece];
}

inline unsigned int Board::get_nu_moves() const
{
    return m_moves.size();
}

inline unsigned int Board::get_nu_onboard_pieces() const
{
    return m_nu_onboard_pieces_all;
}

inline unsigned int Board::get_nu_onboard_pieces(Color c) const
{
    return m_nu_onboard_pieces[c];
}

inline const MovePoints& Board::get_move_points(Move mv) const
{
    return m_board_const->get_move_points(mv);
}

inline unsigned int Board::get_nu_pieces() const
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
    return m_pieces_left[c];
}

inline Move Board::get_played_move(Point p) const
{
    return m_played_move[p];
}

inline PointState Board::get_point_state(Point p) const
{
    LIBBOARDGAME_ASSERT(is_onboard(p));
    return PointState(m_point_state[p].to_int());
}

inline unsigned int Board::get_points(Color c) const
{
    unsigned int total_piece_points = m_board_const->get_total_piece_points();
    if (m_variant == variant_junior)
        total_piece_points *= 2;
    return total_piece_points - get_points_left(c);
}

inline unsigned int Board::get_points_with_bonus(Color c) const
{
    return get_points(c) + get_bonus(c);
}

inline Color Board::get_previous(Color c) const
{
    return c.get_previous(m_nu_colors);
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
    return m_to_play;
}

inline const PieceTransforms& Board::get_transforms() const
{
    return m_board_const->get_transforms();
}

inline Variant Board::get_variant() const
{
    return m_variant;
}

inline bool Board::has_setup() const
{
    for (ColorIterator i(m_nu_colors); i; ++i)
        if (! m_setup.placements[*i].empty())
            return true;
    return false;
}

inline void Board::init(const Setup* setup)
{
    init(m_variant, setup);
}

inline bool Board::is_attach_point(Point p, Color c) const
{
    return m_is_attach_point[c][p];
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
    return m_nu_onboard_pieces[c] == 0;
}

inline bool Board::is_forbidden(Point p, Color c) const
{
    return m_forbidden[c][p];
}

inline const Grid<bool>& Board::is_forbidden(Color c) const
{
    return m_forbidden[c];
}

inline bool Board::is_forbidden(Color c, Move mv) const
{
    const MovePoints& points = get_move_points(mv);
    auto end = points.end();
    auto i = points.begin();
    LIBBOARDGAME_ASSERT(i != end);
    do
    {
        if (m_forbidden[c][*i])
            return true;
        ++i;
    }
    while (i != end);
    return false;
}

inline bool Board::is_legal(Move mv) const
{
    return is_legal(m_to_play, mv);
}

inline bool Board::is_legal(Color c, Move mv) const
{
    if (mv.is_pass())
        return true;
    const MovePoints& points = get_move_points(mv);
    bool has_attach_point = false;
    auto i = points.begin();
    auto end = points.end();
    LIBBOARDGAME_ASSERT(i != end);
    do
    {
        if (m_forbidden[c][*i])
            return false;
        if (is_attach_point(*i, c))
            has_attach_point = true;
        ++i;
    }
    while (i != end);
    if (has_attach_point)
        return true;
    if (! is_first_piece(c))
        return false;
    i = points.begin();
    do
    {
        if (is_colorless_starting_point(*i)
            || (is_colored_starting_point(*i)
                && get_starting_point_color(*i) == c))
            return true;
        ++i;
    }
    while (i != end);
    return false;
}

inline bool Board::is_onboard(Point p) const
{
    return m_geometry->is_onboard(p);
}

inline bool Board::is_piece_left(Color c, Piece piece) const
{
    LIBBOARDGAME_ASSERT(piece.to_int() < get_nu_pieces());
    return m_nu_left_piece[c][piece] > 0;
}

inline bool Board::is_same_player(Color c1, Color c2) const
{
    return (c1 == c2 || c1 == m_second_color[c2]);
}

inline void Board::place(Color c, Move mv)
{
    LIBBOARDGAME_ASSERT(mv.is_regular());
    const MoveInfo& info = m_board_const->get_move_info(mv);
    const MoveInfoExt& info_ext = m_board_const->get_move_info_ext(mv);
    Piece piece = info.piece;
    LIBBOARDGAME_ASSERT(m_nu_left_piece[c][piece] > 0);
    if (--m_nu_left_piece[c][piece] == 0)
        m_pieces_left[c].remove(piece);
    ++m_nu_onboard_pieces_all;
    ++m_nu_onboard_pieces[c];
    auto i = info.points.begin();
    auto end = info.points.end();
    LIBBOARDGAME_ASSERT(i != end);
    do
    {
        m_point_state[*i] = c;
        m_played_move[*i] = mv;
        LIBPENTOBI_FOREACH_COLOR(c, m_forbidden[c][*i] = true);
        ++i;
    }
    while (i != end);
    i = info_ext.adj_points.begin();
    end = info_ext.adj_points.end();
    LIBBOARDGAME_ASSERT(i != end);
    do
    {
        m_forbidden[c][*i] = true;
        ++i;
    }
    while (i != end);
    i = info_ext.attach_points.begin();
    end = info_ext.attach_points.end();
    LIBBOARDGAME_ASSERT(i != end);
    do
    {
        if (! m_is_attach_point[c][*i])
        {
            m_is_attach_point[c][*i] = true;
            m_attach_points[c].push_back(*i);
        }
        ++i;
    }
    while (i != end);
}

inline void Board::play(ColorMove move)
{
    play(move.color, move.move);
}

inline void Board::play(Color c, Move mv)
{
    if (! mv.is_pass())
        play_nonpass(c, mv);
    else
        play_pass(c);
}

inline void Board::play_nonpass(Color c, Move mv)
{
    place(c, mv);
    m_moves.push_back(ColorMove(c, mv));
    m_to_play = get_next(c);
}

inline void Board::play_pass(Color c)
{
    m_moves.push_back(ColorMove(c, Move::pass()));
    m_to_play = get_next(c);
}

inline void Board::play(Move move)
{
    play(m_to_play, move);
}

inline void Board::set_to_play(Color c)
{
    m_to_play = c;
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
