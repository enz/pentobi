//-----------------------------------------------------------------------------
/** @file libpentobi_base/Board.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOARD_H
#define LIBPENTOBI_BASE_BOARD_H

#include "BoardConst.h"
#include "ColorMap.h"
#include "ColorMove.h"
#include "FullGrid.h"
#include "GameVariant.h"
#include "Geometry.h"
#include "MoveMarker.h"
#include "PointStateExt.h"

namespace libpentobi_base {

using libboardgame_util::NullTermList;
using libpentobi_base::ColorMap;
using libpentobi_base::PointState;
using libpentobi_base::PointStateExt;

//-----------------------------------------------------------------------------

class StartingPoints
{
public:
    static const unsigned int max_starting_points = 6;

    void init(GameVariant game_variant, const Geometry& geometry);

    bool is_colored_starting_point(Point p) const;

    bool is_colorless_starting_point(Point p) const;

    Color get_starting_point_color(Point p) const;

    const ArrayList<Point,StartingPoints::max_starting_points>&
                                             get_starting_points(Color c) const;

private:
    Grid<bool> m_is_colored_starting_point;

    Grid<bool> m_is_colorless_starting_point;

    Grid<Color> m_starting_point_color;

    ColorMap<ArrayList<Point,max_starting_points>> m_starting_points;

    void add_colored_starting_point(unsigned int x, unsigned int y, Color c);

    void add_colorless_starting_point(unsigned int x, unsigned int y);
};

inline Color StartingPoints::get_starting_point_color(Point p) const
{
    LIBBOARDGAME_ASSERT(m_is_colored_starting_point[p]);
    return m_starting_point_color[p];
}

inline const ArrayList<Point,StartingPoints::max_starting_points>&
                              StartingPoints::get_starting_points(Color c) const
{
    return m_starting_points[c];
}

inline bool StartingPoints::is_colored_starting_point(Point p) const
{
    return m_is_colored_starting_point[p];
}

inline bool StartingPoints::is_colorless_starting_point(Point p) const
{
    return m_is_colorless_starting_point[p];
}

//-----------------------------------------------------------------------------

class Board
{
public:
    typedef FullGrid<PointStateExt> PointStateGrid;

    /** Iterator over all points on the board. */
    class Iterator
        : public GeometryIterator
    {
        friend class Board;

    public:
        Iterator(const Board& bd);
    };

    static const unsigned int max_pieces = BoardConst::max_pieces;

    /** Maximum number of moves for a player in a game.
        Assumes that a player is only allowed to pass if she has no legal
        moves and allows an extra pass move per player at the end of a game to
        indicate that the game is over. */
    static const unsigned int max_player_moves = max_pieces + 1;

    static const unsigned int max_game_moves = Color::range * max_player_moves;

    /** Use ANSI escape sequences for colored text output in operator>> */
    static bool color_output;

    Board(GameVariant game_variant);

    GameVariant get_game_variant() const;

    unsigned int get_nu_colors() const;

    unsigned int get_nu_pieces() const;

    const PieceTransforms& get_transforms() const;

    /** Get the state of an on-board point. */
    PointState get_point_state(Point p) const;

    /** Get the state of an on-board or off-board point. */
    PointStateExt get_point_state_ext(Point p) const;

    bool is_empty(Point p) const;

    const PointStateGrid& get_grid() const;

    Color get_to_play() const;

    Color get_effective_to_play() const;

    const ArrayList<unsigned int, max_pieces>& get_pieces_left(Color c) const;

    bool is_piece_left(Color c, const Piece& piece) const;

    unsigned int get_points(Color c) const;

    unsigned int get_bonus(Color c) const;

    unsigned int get_points_with_bonus(Color c) const;

    unsigned int get_points_left(Color c) const;

    Move get_played_move(Point p) const;

    /** Is a point a potential attachment point for a color.
        Does not check if the point is forbidden.. */
    bool is_attach_point(Point p, Color c) const;

    void init();

    void init(GameVariant game_variant);

    void copy_from(const Board& bd);

    void play(Color c, Move move);

    void play(ColorMove move);

    void play(Move mv);

    void undo();

    void set_to_play(Color c);

    void write(ostream& out, bool mark_last_move = true) const;

    unsigned int get_nu_moves() const;

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

    const FullGrid<bool>& is_forbidden(Color c) const;

    /** Check that no points of move are already occupied or adjacent to own
        color.
        Does not check if the move is diagonally adjacent to an existing
        occupied point of the same color. */
    bool is_forbidden(Color c, Move mv) const;

    const BoardConst& get_board_const() const;

    BoardType get_board_type() const;

    unsigned int get_adj_status_index(Point p, Color c) const;

    const vector<Move>& get_moves(unsigned int piece, Point p,
                                  unsigned int adj_status_index) const;

    /** Get score.
        The score is the number of points for a color minus the number of
        points of the opponent (or the average score of the opponents if there
        are more than two players).
        @param c
        @param[out] game_result The game result from the point of view of the
        color: 1=win, 0=loss, 0.5=draw
        @todo The score should be a floating point type because averaging the
        points of the opponents in the four-player version leads to fractional
        points. */
    int get_score(Color c, double& game_result) const;

    const Geometry& get_geometry() const;

    string to_string(Move mv, bool only_points = false) const;

    bool find_move(const MovePoints& points, Move& move) const;

    const Piece& get_piece(unsigned int n) const;

    bool get_piece_index_by_name(const string& name,
                                 unsigned int& index) const;

    bool get_piece_by_name(const string& name, const Piece*& piece) const;

    const MovePoints& get_move_points(Move mv) const;

    const Piece& get_move_piece(Move mv) const;

    const MoveInfo& get_move_info(Move move) const;

    bool is_colored_starting_point(Point p) const;

    bool is_colorless_starting_point(Point p) const;

    Color get_starting_point_color(Point p) const;

    const ArrayList<Point,StartingPoints::max_starting_points>&
                                             get_starting_points(Color c) const;

    bool is_onboard(Point p) const;

    /** Get the second color in game variants in which a player plays two
        colors.
        @return The second color of the player that plays color c, or c if
        the player plays only one color in the current game variant. */
    Color get_second_color(Color c) const;

    bool is_same_player(Color c1, Color c2) const;

private:
    GameVariant m_game_variant;

    unsigned int m_nu_colors;

    const BoardConst* m_board_const;

    const Geometry* m_geometry;

    Color m_to_play;

    PointStateGrid m_point_state;

    ColorMap<FullGrid<bool>> m_forbidden;

    ColorMap<Grid<bool>> m_is_attach_point;

    Grid<Move> m_played_move;

    ColorMap<ArrayList<unsigned int, max_pieces>> m_pieces_left;

    /** See get_second_color() */
    ColorMap<Color> m_second_color;

    ArrayList<ColorMove, max_game_moves> m_moves;

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

inline const BoardConst& Board::get_board_const() const
{
    return *m_board_const;
}

inline BoardType Board::get_board_type() const
{
    return m_board_const->get_board_type();
}

inline GameVariant Board::get_game_variant() const
{
    return m_game_variant;
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

inline const Piece& Board::get_move_piece(Move mv) const
{
    return get_piece(get_move_info(mv).piece);
}

inline const vector<Move>& Board::get_moves(unsigned int piece, Point p,
                                            unsigned int adj_status_index) const
{
    return m_board_const->get_moves(piece, p, adj_status_index);
}

inline unsigned int Board::get_nu_colors() const
{
    return m_nu_colors;
}

inline unsigned int Board::get_nu_moves() const
{
    return m_moves.size();
}

inline const MovePoints& Board::get_move_points(Move mv) const
{
    return m_board_const->get_move_points(mv);
}

inline unsigned int Board::get_nu_pieces() const
{
    return m_board_const->get_nu_pieces();
}

inline const Piece& Board::get_piece(unsigned int n) const
{
    return m_board_const->get_piece(n);
}

inline bool Board::get_piece_by_name(const string& name,
                                     const Piece*& piece) const
{
    unsigned int index;
    if (! get_piece_index_by_name(name, index))
        return false;
    piece = &get_piece(index);
    return true;
}

inline bool Board::get_piece_index_by_name(const string& name,
                                           unsigned int& index) const
{
    return m_board_const->get_piece_index_by_name(name, index);
}

inline const ArrayList<unsigned int, Board::max_pieces>&
                                          Board::get_pieces_left(Color c) const
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

inline PointStateExt Board::get_point_state_ext(Point p) const
{
    return m_point_state[p];
}

inline Color Board::get_second_color(Color c) const
{
    return m_second_color[c];
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

inline void Board::init()
{
    init(m_game_variant);
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

inline bool Board::is_forbidden(Point p, Color c) const
{
    return m_forbidden[c][p];
}

inline const FullGrid<bool>& Board::is_forbidden(Color c) const
{
    return m_forbidden[c];
}

inline bool Board::is_forbidden(Color c, Move mv) const
{
    const MovePoints& points = get_move_points(mv);
    for (auto i = points.begin(); i != points.end(); ++i)
        if (m_forbidden[c][*i])
            return true;
    return false;
}

inline bool Board::is_legal(Move mv) const
{
    return is_legal(m_to_play, mv);
}

inline bool Board::is_onboard(Point p) const
{
    return m_geometry->is_onboard(p);
}

inline bool Board::is_same_player(Color c1, Color c2) const
{
    return (c1 == c2 || c1 == m_second_color[c2]);
}

inline void Board::play(ColorMove move)
{
    play(move.color, move.move);
}

inline void Board::play(Move move)
{
    play(m_to_play, move);
}

inline void Board::set_to_play(Color c)
{
    m_to_play = c;
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
