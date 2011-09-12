//-----------------------------------------------------------------------------
/** @file libpentobi_base/Board.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_BOARD_H
#define LIBPENTOBI_BASE_BOARD_H

#include "BoardConst.h"
#include "ColorMove.h"
#include "FullGrid.h"
#include "GameVariant.h"
#include "Geometry.h"
#include "MoveMarker.h"
#include "libpentobi_base/ColorMap.h"
#include "libpentobi_base/PointStateExt.h"

namespace libpentobi_base {

using libpentobi_base::ColorMap;
using libpentobi_base::PointState;
using libpentobi_base::PointStateExt;

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

    static const unsigned int nu_pieces = BoardConst::nu_pieces;

    /** Maximum number of moves for a player in a game.
        Assumes that a player is only allowed to pass if she has no legal
        moves and allows an extra pass move per player at the end of a game to
        indicate that the game is over. */
    static const unsigned int max_player_moves = nu_pieces + 1;

    static const unsigned int max_game_moves = Color::range * max_player_moves;

    /** Use ANSI escape sequences for colored text output in operator>> */
    static bool color_output;

    static Point get_starting_point(GameVariant game_variant, Color c);

    static bool is_starting_point(Point p, GameVariant game_variant,
                                  Color& color);

    Board(GameVariant game_variant);

    GameVariant get_game_variant() const;

    unsigned int get_nu_colors() const;

    /** Get the state of an on-board point. */
    PointState get_point_state(Point p) const;

    /** Get the state of an on-board or off-board point. */
    PointStateExt get_point_state_ext(Point p) const;

    const PointStateGrid& get_grid() const;

    Color get_to_play() const;

    Color get_effective_to_play() const;

    const ArrayList<unsigned int, nu_pieces>& get_pieces_left(Color c) const;

    bool is_piece_left(Color c, const Piece& piece) const;

    unsigned int get_points(Color c) const;

    unsigned int get_bonus(Color c) const;

    unsigned int get_points_with_bonus(Color c) const;

    unsigned int get_points_left(Color c) const;

    Move get_played_move(Point p) const;

    bool has_diag(Point p, Color c) const;

    void init();

    void init(GameVariant game_variant);

    void copy_from(const Board& bd);

    void play(Color c, Move move);

    void play(ColorMove move);

    void play(Move mv);

    void undo();

    void set_to_play(Color c);

    void write(ostream& out) const;

    unsigned int get_nu_moves() const;

    ColorMove get_move(unsigned int n) const;

    /** Generate all moves for one player.
        The generated moves do not include the pass move. */
    void gen_moves(Color c, vector<Move>& moves) const;

    bool has_moves(Color c) const;

    /** Check that no color has any moves left. */
    bool is_game_over() const;

    bool is_legal(Color c, Move mv) const;

    bool is_legal(Move mv) const;

    /** Check that point is not already occupied or adjacent to own color.
        @param c
        @param p The point. Off-board points are allowed and return true. */
    bool is_forbidden(Color c, Point p) const;

    const FullGrid<bool>& is_forbidden(Color c) const;

    /** Check that no points of move are already occupied or adjacent to own
        color.
        Does not check if the move is diagonally adjacent to an existing
        occupied point of the same color. */
    bool is_forbidden(Color c, Move mv) const;

    const BoardConst& get_board_const() const;

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

    unsigned int get_size() const;

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

    Point get_starting_point(Color c) const;

    bool is_starting_point(Point p, Color& color) const;

    bool is_onboard(Point p) const;

    bool is_same_player(Color c1, Color c2) const;

private:
    GameVariant m_game_variant;

    unsigned int m_sz;

    unsigned int m_nu_colors;

    const BoardConst* m_board_const;

    const Geometry* m_geometry;

    Color m_to_play;

    PointStateGrid m_point_state;

    ColorMap<FullGrid<bool>> m_forbidden;

    ColorMap<FullGrid<bool>> m_has_diag;

    Grid<Move> m_played_move;

    ColorMap<ArrayList<unsigned int, nu_pieces>> m_pieces_left;

    ArrayList<ColorMove, max_game_moves> m_moves;

    /** Local variable during move generation.
        Reused for efficiency. */
    mutable MoveMarker m_marker;

    void gen_moves(Color c, Point p, MoveMarker& marker,
                   vector<Move>& moves) const;

    void gen_moves(Color c, Point p, unsigned int adj_status_index,
                   MoveMarker& marker, vector<Move>& moves) const;

    bool has_moves(Color c, Point p) const;

    void write_pieces_left(ostream& out, Color c, unsigned int begin,
                           unsigned int end) const;

    void write_color_info_line1(ostream& out, Color c, const char* name,
                                char color_char, const char* color_esc_sequence,
                                unsigned int y) const;

    void write_color_info_line2(ostream& out, Color c, unsigned int y) const;

    void write_color_info_line3(ostream& out, Color c, unsigned int y) const;
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

inline const BoardConst& Board::get_board_const() const
{
    return *m_board_const;
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

inline const ArrayList<unsigned int, Board::nu_pieces>&
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

inline unsigned int Board::get_size() const
{
    return m_sz;
}

inline Point Board::get_starting_point(Color c) const
{
    return get_starting_point(m_game_variant, c);
}

inline Color Board::get_to_play() const
{
    return m_to_play;
}

inline bool Board::has_diag(Point p, Color c) const
{
    return m_has_diag[c][p];
}

inline void Board::init()
{
    init(m_game_variant);
}

inline bool Board::is_forbidden(Color c, Point p) const
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
    return p.is_onboard(m_sz);
}

inline bool Board::is_same_player(Color c1, Color c2) const
{
    return (c1 == c2 ||
            (m_game_variant == game_variant_classic_2
             && (c1.to_int() % 2) == (c2.to_int() % 2)));
}

inline bool Board::is_starting_point(Point p, Color& color) const
{
    return is_starting_point(p, m_game_variant, color);
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
