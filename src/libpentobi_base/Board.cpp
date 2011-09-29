//-----------------------------------------------------------------------------
/** @file libpentobi_base/Board.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Board.h"
#include "libboardgame_util/Unused.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

namespace {

void write_x_coord(ostream& out, unsigned int sz)
{
    out << "  ";
    char c = 'A';
    for (unsigned int x = 0; x < sz; ++x, ++c)
        out << ' ' << c;
    out << '\n';
}

}

//-----------------------------------------------------------------------------

bool Board::color_output = false;

Board::Board(GameVariant game_variant)
{
    init(game_variant);
}

void Board::copy_from(const Board& bd)
{
    init(bd.get_game_variant());
    for (unsigned int i = 0; i < bd.get_nu_moves(); ++i)
        play(bd.get_move(i));
}

void Board::gen_moves(Color c, ArrayList<Move, Move::range>& moves) const
{
    moves.clear();
    bool is_first_move = (m_pieces_left[c].size() == nu_pieces);
    if (is_first_move)
    {
        if (! m_forbidden[c][get_starting_point(c)])
            gen_moves(c, get_starting_point(c), m_marker, moves);
    }
    else
    {
        for (Iterator i(*this); i; ++i)
            if (has_diag(*i, c) && ! m_forbidden[c][*i])
                gen_moves(c, *i, get_adj_status_index(*i, c), m_marker, moves);
    }
    m_marker.clear(moves);
}

void Board::gen_moves(Color c, Point p, MoveMarker& marker,
                      ArrayList<Move, Move::range>& moves) const
{
    BOOST_FOREACH(unsigned int i, m_pieces_left[c])
    {
        BOOST_FOREACH(Move mv, m_board_const->get_moves(i, p))
        {
            if (marker[mv])
                continue;
            if (! is_forbidden(c, mv))
            {
                moves.push_back(mv);
                marker.set(mv);
            }
        }
    }
}

void Board::gen_moves(Color c, Point p, unsigned int adj_status_index,
                      MoveMarker& marker,
                      ArrayList<Move, Move::range>& moves) const
{
    BOOST_FOREACH(unsigned int i, m_pieces_left[c])
    {
        BOOST_FOREACH(Move mv, m_board_const->get_moves(i, p, adj_status_index))
        {
            if (marker[mv])
                continue;
            if (! is_forbidden(c, mv))
            {
                moves.push_back(mv);
                marker.set(mv);
            }
        }
    }
}

unsigned int Board::get_adj_status_index(Point p, Color c) const
{
    bool s0 = is_forbidden(p.get_neighbor(Direction::get_enum_adj(0)), c);
    bool s1 = is_forbidden(p.get_neighbor(Direction::get_enum_adj(1)), c);
    bool s2 = is_forbidden(p.get_neighbor(Direction::get_enum_adj(2)), c);
    bool s3 = is_forbidden(p.get_neighbor(Direction::get_enum_adj(3)), c);
    return m_board_const->get_adj_status_index(s0, s1, s2, s3);
}

unsigned int Board::get_bonus(Color c) const
{
    unsigned int bonus = 0;
    if (m_pieces_left[c].size() == 0)
    {
        bonus = 15;
        for (unsigned int i = get_nu_moves(); i > 0; --i)
        {
            ColorMove mv = get_move(i - 1);
            if (mv.color == c)
            {
                if (! mv.move.is_pass())
                {
                    const MoveInfo& info = get_move_info(mv.move);
                    if (get_piece(info.piece).get_size() == 1)
                        bonus += 5;
                }
                break;
            }
        }
    }
    return bonus;
}

Color Board::get_effective_to_play() const
{
    Color c = m_to_play;
    for (unsigned int i = 0; i < m_nu_colors; ++i)
    {
        if (has_moves(c))
            return c;
        c = c.get_next(m_nu_colors);
    }
    return m_to_play;
}

unsigned int Board::get_points(Color c) const
{
    return BoardConst::total_piece_points - get_points_left(c);
}

unsigned int Board::get_points_with_bonus(Color c) const
{
    return get_points(c) + get_bonus(c);
}

unsigned int Board::get_points_left(Color c) const
{
    unsigned int n = 0;
    BOOST_FOREACH(unsigned int i, m_pieces_left[c])
        n += get_piece(i).get_size();
    return n;
}

int Board::get_score(Color c, double& game_result) const
{
    if (m_game_variant == game_variant_duo)
    {
        unsigned int points0 = get_points_with_bonus(Color(0));
        unsigned int points1 = get_points_with_bonus(Color(1));
        if (c == Color(0))
        {
            if (points0 > points1)
                game_result = 1;
            else if (points0 < points1)
                game_result = 0;
            else
                game_result = 0.5;
            return points0 - points1;
        }
        else
        {
            if (points1 > points0)
                game_result = 1;
            else if (points1 < points0)
                game_result = 0;
            else
                game_result = 0.5;
            return points1 - points0;
        }
    }
    else if (m_game_variant == game_variant_classic)
    {
        unsigned int points = get_points_with_bonus(c);
        int score = 0;
        unsigned int max_opponent_points = 0;
        for (ColorIterator i(m_nu_colors); i; ++i)
            if (*i != c)
            {
                unsigned int points = get_points_with_bonus(*i);
                score -= points;
                max_opponent_points = max(max_opponent_points, points);
            }
        score = score / 3 + points;
        if (points > max_opponent_points)
            game_result = 1;
        else if (points < max_opponent_points)
            game_result = 0;
        else
            game_result = 0.5;
        return score;
    }
    else
    {
        LIBBOARDGAME_ASSERT(m_game_variant == game_variant_classic_2);
        unsigned int points0 =
            get_points_with_bonus(Color(0)) + get_points_with_bonus(Color(2));
        unsigned int points1 =
            get_points_with_bonus(Color(1)) + get_points_with_bonus(Color(3));
        if (c == Color(0) || c == Color(2))
        {
            if (points0 > points1)
                game_result = 1;
            else if (points0 < points1)
                game_result = 0;
            else
                game_result = 0.5;
            return points0 - points1;
        }
        else
        {
            if (points1 > points0)
                game_result = 1;
            else if (points1 < points0)
                game_result = 0;
            else
                game_result = 0.5;
            return points1 - points0;
        }
    }
}

Point Board::get_starting_point(GameVariant game_variant, Color c)
{
    if  (game_variant == game_variant_classic
         || game_variant == game_variant_classic_2)
    {
        unsigned int sz = 20;
        if (c == Color(0))
            return Point(0, sz - 1);
        if (c == Color(1))
            return Point(sz - 1, sz - 1);
        if (c == Color(2))
            return Point(sz - 1, 0);
        LIBBOARDGAME_ASSERT(c == Color(3));
        return Point(0, 0);
    }
    else
    {
        unsigned int sz = 14;
        LIBBOARDGAME_ASSERT(game_variant == game_variant_duo);
        if (c == Color(0))
            return Point(4, sz - 4 - 1);
        LIBBOARDGAME_ASSERT(c == Color(1));
        return Point(sz - 4 - 1, 4);
    }
}

bool Board::has_moves(Color c) const
{
    bool is_first_move = (m_pieces_left[c].size() == nu_pieces);
    for (Iterator i(*this); i; ++i)
        if (! m_forbidden[c][*i]
            && (has_diag(*i, c)
                || (is_first_move && *i == get_starting_point(c))))
            if (has_moves(c, *i))
                return true;
    return false;
}

bool Board::has_moves(Color c, Point p) const
{
    BOOST_FOREACH(unsigned int i, m_pieces_left[c])
    {
        BOOST_FOREACH(Move mv, m_board_const->get_moves(i, p))
        {
            const MovePoints& points = get_move_points(mv);
            bool is_legal = true;
            BOOST_FOREACH(Point p2, points)
                if (m_forbidden[c][p2])
                {
                    is_legal = false;
                    break;
                }
            if (is_legal)
                return true;
        }
    }
    return false;
}

void Board::init(GameVariant game_variant)
{
    m_game_variant = game_variant;
    if  (game_variant == game_variant_classic
         || game_variant == game_variant_classic_2)
    {
        m_sz = 20;
        m_nu_colors = 4;
    }
    else
    {
        LIBBOARDGAME_ASSERT(game_variant == game_variant_duo);
        m_sz = 14;
        m_nu_colors = 2;
    }
    m_board_const = &BoardConst::get(m_sz);
    m_geometry = Geometry::get(m_sz);
    m_point_state.init(*m_geometry);
    m_point_state.fill_all(PointStateExt::offboard());
    m_point_state.fill_onboard(PointState::empty());
    m_played_move.init(*m_geometry);
    m_played_move.fill(Move::null());
    for (ColorIterator i(m_nu_colors); i; ++i)
    {
        if (game_variant == game_variant_classic_2)
            m_second_color[*i] =
                (*i).get_next(m_nu_colors).get_next(m_nu_colors);
        else
            m_second_color[*i] = *i;
        m_forbidden[*i].init(*m_geometry);
        m_forbidden[*i].fill_all(true);
        m_forbidden[*i].fill_onboard(false);
        m_has_diag[*i].init(*m_geometry);
        m_has_diag[*i].fill_onboard(false);
        m_pieces_left[*i].clear();
        for (unsigned int j = 0; j < nu_pieces; ++j)
            m_pieces_left[*i].push_back(j);
    }
    m_to_play = Color(0);
    m_moves.clear();
}

bool Board::is_game_over() const
{
    for (ColorIterator i(m_nu_colors); i; ++i)
        if (has_moves(*i))
            return false;
    return true;
}

bool Board::is_legal(Color c, Move mv) const
{
    if (mv.is_pass())
        return true;
    const MovePoints& points = get_move_points(mv);
    bool is_diag_to_piece = false;
    BOOST_FOREACH(Point p, points)
    {
        if (m_forbidden[c][p])
            return false;
        if (has_diag(p, c))
            is_diag_to_piece = true;
    }
    bool is_first_move = (m_pieces_left[c].size() == nu_pieces);
    return (is_diag_to_piece ||
            (is_first_move && points.contains(get_starting_point(c))));
}

bool Board::is_piece_left(Color c, const Piece& piece) const
{
    BOOST_FOREACH(unsigned int i, m_pieces_left[c])
        if (&get_piece(i) == &piece)
            return true;
    return false;
}

bool Board::is_starting_point(Point p, GameVariant game_variant, Color& color)
{
    unsigned int nu_colors = (game_variant == game_variant_duo ? 2 : 4);
    for (ColorIterator i(nu_colors); i; ++i)
        if (p == get_starting_point(game_variant, *i))
        {
            color = *i;
            return true;
        }
    return false;
}

void Board::play(Color c, Move mv)
{
    LIBBOARDGAME_ASSERT(! mv.is_null());
    if (! mv.is_pass())
    {
        const MoveInfo& info = m_board_const->get_move_info(mv);
        LIBBOARDGAME_ASSERT(m_pieces_left[c].contains(info.piece));
        m_pieces_left[c].remove(info.piece);
        for (auto i = info.points.begin(); i != info.points.end(); ++i)
        {
            m_point_state[*i] = c;
            m_played_move[*i] = mv;
            for (ColorIterator j(m_nu_colors); j; ++j)
                m_forbidden[*j][*i] = true;
            LIBBOARDGAME_FOREACH_DIAG(*i, p_diag,
                m_has_diag[c][p_diag] = true;)
        }
        for (auto i = info.adj_points.begin(); i != info.adj_points.end(); ++i)
            m_forbidden[c][*i] = true;
    }
    m_moves.push_back(ColorMove(c, mv));
    m_to_play = c.get_next(m_nu_colors);
}

string Board::to_string(Move mv, bool only_points) const
{
    if (mv.is_null())
        return "NULL";
    if (mv.is_pass())
        return "pass";
    const MoveInfo& info = get_move_info(mv);
    ostringstream s;
    if (! only_points)
        s << '[' << get_piece(info.piece).get_name() << "] ";
    s << info.points;
    return s.str();
}

void Board::undo()
{
    LIBBOARDGAME_ASSERT(get_nu_moves() > 0);
    ArrayList<ColorMove, max_game_moves> moves = m_moves;
    moves.pop_back();
    init();
    for (unsigned int i = 0; i < moves.size(); ++i)
        play(moves[i]);
}

void Board::write(ostream& out) const
{
    ColorMap<char> color_char;
    color_char[Color(0)] = 'X';
    color_char[Color(1)] = 'O';
    color_char[Color(2)] = '#';
    color_char[Color(3)] = '@';
    ColorMap<const char*> color_esc_sequence;
    ColorMap<const char*> color_esc_sequence_text;
    ColorMap<const char*> color_name;
    if (m_game_variant == game_variant_duo)
    {
        color_name[Color(0)] = "Blue";
        color_name[Color(1)] = "Green";
        color_esc_sequence[Color(0)] = "\x1B[1;34;47m";
        color_esc_sequence[Color(1)] = "\x1B[1;32;47m";
        color_esc_sequence_text[Color(0)] = "\x1B[1;34m";
        color_esc_sequence_text[Color(1)] = "\x1B[1;32m";
    }
    else
    {
        color_name[Color(0)] = "Blue";
        color_name[Color(1)] = "Yellow";
        color_name[Color(2)] = "Red";
        color_name[Color(3)] = "Green";
        color_esc_sequence[Color(0)] = "\x1B[1;34;47m";
        color_esc_sequence[Color(1)] = "\x1B[1;33;47m";
        color_esc_sequence[Color(2)] = "\x1B[1;31;47m";
        color_esc_sequence[Color(3)] = "\x1B[1;32;47m";
        color_esc_sequence_text[Color(0)] = "\x1B[1;34m";
        color_esc_sequence_text[Color(1)] = "\x1B[1;33m";
        color_esc_sequence_text[Color(2)] = "\x1B[1;31m";
        color_esc_sequence_text[Color(3)] = "\x1B[1;32m";
    }
    write_x_coord(out, m_sz);
    for (unsigned int y = m_sz - 1; ; --y)
    {
        if (y < 9)
            out << ' ';
        out << (y + 1) << ' ';
        for (unsigned int x = 0; x < m_sz; ++x)
        {
            if (x > 0)
            {
                if (color_output)
                    out << "\x1B[47m";
                out << ' ';
            }
            Point p(x, y);
            PointState s = get_point_state(p);
            if (s.is_empty())
            {
                Color color;
                if (is_starting_point(p, color))
                {
                    if (color_output)
                        out << color_esc_sequence[color];
                    out << '+';
                }
                else
                {
                    if (color_output)
                        out << "\x1B[1;30;47m";
                    out << '.';
                }
            }
            else
            {
                Color color = s.to_color();
                if (color_output)
                    out << color_esc_sequence[color];
                out << color_char[color];
            }
        }
        if (color_output)
            out << "\x1B[0m";
        out << ' ' << (y + 1);
        if (y == m_sz - 1)
            write_color_info_line1(out, Color(0), color_name[Color(0)],
                                   color_char[Color(0)],
                                   color_esc_sequence_text[Color(0)], y);
        else if (y == m_sz - 2)
            write_color_info_line2(out, Color(0), y);
        else if (y == m_sz - 3)
            write_color_info_line3(out, Color(0), y);
        else if (y == m_sz - 5)
            write_color_info_line1(out, Color(1), color_name[Color(1)],
                                   color_char[Color(1)],
                                   color_esc_sequence_text[Color(1)], y);
        else if (y == m_sz - 6)
            write_color_info_line2(out, Color(1), y);
        else if (y == m_sz - 7)
            write_color_info_line3(out, Color(1), y);
        else if (y == m_sz - 9 && m_nu_colors > 2)
            write_color_info_line1(out, Color(2), color_name[Color(2)],
                                   color_char[Color(2)],
                                   color_esc_sequence_text[Color(2)], y);
        else if (y == m_sz - 10 && m_nu_colors > 2)
            write_color_info_line2(out, Color(2), y);
        else if (y == m_sz - 11 && m_nu_colors > 2)
            write_color_info_line3(out, Color(2), y);
        else if (y == m_sz - 13 && m_nu_colors > 3)
            write_color_info_line1(out, Color(3), color_name[Color(3)],
                                   color_char[Color(3)],
                                   color_esc_sequence_text[Color(3)], y);
        else if (y == m_sz - 14 && m_nu_colors > 3)
            write_color_info_line2(out, Color(3), y);
        else if (y == m_sz - 15 && m_nu_colors > 3)
            write_color_info_line3(out, Color(3), y);
        out << '\n';
        if (y == 0)
            break;
    }
    write_x_coord(out, m_sz);
}

void Board::write_color_info_line1(ostream& out, Color c, const char* name,
                                   char color_char,
                                   const char* color_esc_sequence,
                                   unsigned int y) const
{
    if (y + 1 < 10)
        out << "   ";
    else
        out << "  ";
    if (color_output)
        out << color_esc_sequence;
    out << name << "(" << color_char << "): " << get_points(c);
    unsigned int bonus = get_bonus(c);
    if (bonus > 0)
        out << " (+" << bonus << ')';
    if (get_to_play() == c)
        out << " *";
    if (color_output)
        out << "\x1B[0m";
}

void Board::write_color_info_line2(ostream& out, Color c,
                                   unsigned int y) const
{
    if (y + 1 < 10)
        out << "  ";
    else
        out << ' ';
    write_pieces_left(out, c, 0, 10);
}

void Board::write_color_info_line3(ostream& out, Color c,
                                   unsigned int y) const
{
    if (y + 1 < 10)
        out << "  ";
    else
        out << ' ';
    write_pieces_left(out, c, 10, nu_pieces);
}

void Board::write_pieces_left(ostream& out, Color c, unsigned int begin,
                              unsigned int end) const
{
    for (unsigned int i = begin; i < end; ++i)
        if (i < m_pieces_left[c].size())
            out << ' ' << get_piece(m_pieces_left[c][i]).get_name();
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
