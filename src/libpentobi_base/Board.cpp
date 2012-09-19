//-----------------------------------------------------------------------------
/** @file libpentobi_base/Board.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Board.h"

#include <boost/foreach.hpp>
#include "libboardgame_util/Unused.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

namespace {

void write_x_coord(ostream& out, unsigned int width, unsigned int offset)
{
    for (unsigned int i = 0; i < offset; ++i)
        out << ' ';
    char c = 'A';
    for (unsigned int x = 0; x < width; ++x, ++c)
    {
        if (x < 26)
            out << ' ';
        else
            out << 'a';
        if (x == 26)
            c = 'A';
        out << c;
    }
    out << '\n';
}

void set_color(ostream& out, const char* esc_sequence)
{
    if (Board::color_output)
        out << esc_sequence;
}

} // namespace

//-----------------------------------------------------------------------------

bool Board::color_output = false;

Board::Board(Variant variant)
{
    m_color_char[Color(0)] = 'X';
    m_color_char[Color(1)] = 'O';
    m_color_char[Color(2)] = '#';
    m_color_char[Color(3)] = '@';
    init_variant(variant);
    init();
}

void Board::copy_from(const Board& bd)
{
    if (m_variant != bd.m_variant)
        init_variant(bd.m_variant);
    m_point_state = bd.m_point_state;
    m_played_move = bd.m_played_move;
    m_setup.to_play = bd.m_setup.to_play;
    for (ColorIterator i(m_nu_colors); i; ++i)
    {
        m_forbidden[*i] = bd.m_forbidden[*i];
        m_is_attach_point[*i] = bd.m_is_attach_point[*i];
        m_attach_points[*i] = bd.m_attach_points[*i];
        m_pieces_left[*i] = bd.m_pieces_left[*i];
        m_nu_left_piece[*i] = bd.m_nu_left_piece[*i];
        m_setup.placements[*i] = bd.m_setup.placements[*i];
        m_nu_onboard_pieces[*i] = bd.m_nu_onboard_pieces[*i];
    }
    m_moves = bd.m_moves;
    m_nu_onboard_pieces_all = bd.m_nu_onboard_pieces_all;
    m_to_play = bd.m_to_play;
}

void Board::gen_moves(Color c, ArrayList<Move, Move::range>& moves) const
{
    moves.clear();
    if (is_first_piece(c))
    {
        BOOST_FOREACH(Point p, get_starting_points(c))
            if (! m_forbidden[c][p])
                gen_moves(c, p, m_marker, moves);
    }
    else
    {
        for (Iterator i(*this); i; ++i)
            if (is_attach_point(*i, c) && ! m_forbidden[c][*i])
                gen_moves(c, *i, get_adj_status_index(*i, c), m_marker, moves);
    }
    m_marker.clear_all_set_known(moves);
}

void Board::gen_moves(Color c, Point p, MoveMarker& marker,
                      ArrayList<Move, Move::range>& moves) const
{
    BOOST_FOREACH(Piece piece, m_pieces_left[c])
    {
        BOOST_FOREACH(Move mv, m_board_const->get_moves(piece, p))
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
    BOOST_FOREACH(Piece piece, m_pieces_left[c])
    {
        BOOST_FOREACH(Move mv,
                      m_board_const->get_moves(piece, p, adj_status_index))
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

unsigned int Board::get_bonus(Color c) const
{
    unsigned int bonus = 0;
    if (m_variant != variant_junior && m_pieces_left[c].size() == 0)
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
                    if (get_piece_info(info.piece).get_size() == 1)
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
    do
    {
        if (has_moves(c))
            return c;
        c = get_next(c);
    }
    while (c != m_to_play);
    return c;
}

void Board::get_place(Color c, unsigned int& place, bool& is_shared) const
{
    array<unsigned int,Color::range> points_array;
    for (unsigned int i = 0; i < m_nu_colors; ++i)
        points_array[i] = get_points_with_bonus(Color(i));
    unsigned int points = points_array[c.to_int()];
    unsigned int nu_players = get_nu_players(m_variant);
    sort(points_array.begin(), points_array.begin() + nu_players,
         greater<unsigned int>());
    is_shared = false;
    bool found = false;
    for (unsigned int i = 0; i < nu_players; ++i)
        if (points_array[i] == points)
        {
            if (! found)
            {
                place = i;
                found = true;
            }
            else
                is_shared = true;
        }
}

unsigned int Board::get_points_left(Color c) const
{
    unsigned int n = 0;
    BOOST_FOREACH(Piece piece, m_pieces_left[c])
        n += get_nu_left_piece(c, piece) * get_piece_info(piece).get_size();
    return n;
}

int Board::get_score(Color c) const
{
    if (m_variant == variant_duo || m_variant == variant_junior)
    {
        unsigned int points0 = get_points_with_bonus(Color(0));
        unsigned int points1 = get_points_with_bonus(Color(1));
        if (c == Color(0))
            return points0 - points1;
        else
            return points1 - points0;
    }
    else if (m_variant == variant_classic
             || m_variant == variant_trigon
             || m_variant == variant_trigon_3)
    {
        int score = 0;
        for (ColorIterator i(m_nu_colors); i; ++i)
            if (*i != c)
                score -= get_points_with_bonus(*i);
        score =
            get_points_with_bonus(c)
            + score / (static_cast<int>(m_nu_colors) - 1);
        return score;
    }
    else
    {
        LIBBOARDGAME_ASSERT(m_variant == variant_classic_2
                            || m_variant == variant_trigon_2);
        unsigned int points0 =
            get_points_with_bonus(Color(0)) + get_points_with_bonus(Color(2));
        unsigned int points1 =
            get_points_with_bonus(Color(1)) + get_points_with_bonus(Color(3));
        if (c == Color(0) || c == Color(2))
            return points0 - points1;
        else
            return points1 - points0;
    }
}

bool Board::has_moves(Color c) const
{
    bool is_first = is_first_piece(c);
    for (Iterator i(*this); i; ++i)
        if (! m_forbidden[c][*i]
            && (is_attach_point(*i, c)
                || (is_first && get_starting_points(c).contains(*i))))
            if (has_moves(c, *i))
                return true;
    return false;
}

bool Board::has_moves(Color c, Point p) const
{
    BOOST_FOREACH(Piece piece, m_pieces_left[c])
    {
        BOOST_FOREACH(Move mv, m_board_const->get_moves(piece, p))
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

void Board::init(Variant variant, const Setup* setup)
{
    if (variant != m_variant)
        init_variant(variant);

    // If you make changes here, make sure that you also update copy_from()

    m_point_state.fill(PointState::empty());
    m_played_move.fill(Move::null());
    for (ColorIterator i(m_nu_colors); i; ++i)
    {
        m_forbidden[*i].fill(false);
        m_is_attach_point[*i].fill(false);
        m_attach_points[*i].clear();
        m_pieces_left[*i].clear();
        m_nu_onboard_pieces[*i] = 0;
        for (unsigned int j = 0; j < get_nu_pieces(); ++j)
            m_pieces_left[*i].push_back(Piece(j));
        if (variant == variant_junior)
            m_nu_left_piece[*i].fill(2);
        else
            m_nu_left_piece[*i].fill(1);
    }
    m_nu_onboard_pieces_all = 0;
    if (setup == 0)
    {
        m_setup.clear();
        m_to_play = Color(0);
    }
    else
    {
        m_setup = *setup;
        for (ColorIterator i(m_nu_colors); i; ++i)
            BOOST_FOREACH(Move mv, setup->placements[*i])
                place(*i, mv);
        m_to_play = setup->to_play;
    }
    m_moves.clear();
}

void Board::init_variant(Variant variant)
{
    m_variant = variant;
    if (m_variant == variant_duo || m_variant == variant_junior)
    {
        m_color_name[Color(0)] = "Blue";
        m_color_name[Color(1)] = "Green";
        m_color_esc_sequence[Color(0)] = "\x1B[1;34;47m";
        m_color_esc_sequence[Color(1)] = "\x1B[1;32;47m";
        m_color_esc_sequence_text[Color(0)] = "\x1B[1;34m";
        m_color_esc_sequence_text[Color(1)] = "\x1B[1;32m";
    }
    else
    {
        m_color_name[Color(0)] = "Blue";
        m_color_name[Color(1)] = "Yellow";
        m_color_name[Color(2)] = "Red";
        m_color_name[Color(3)] = "Green";
        m_color_esc_sequence[Color(0)] = "\x1B[1;34;47m";
        m_color_esc_sequence[Color(1)] = "\x1B[1;33;47m";
        m_color_esc_sequence[Color(2)] = "\x1B[1;31;47m";
        m_color_esc_sequence[Color(3)] = "\x1B[1;32;47m";
        m_color_esc_sequence_text[Color(0)] = "\x1B[1;34m";
        m_color_esc_sequence_text[Color(1)] = "\x1B[1;33m";
        m_color_esc_sequence_text[Color(2)] = "\x1B[1;31m";
        m_color_esc_sequence_text[Color(3)] = "\x1B[1;32m";
    }
    m_nu_colors = libpentobi_base::get_nu_colors(variant);
    m_board_const = &BoardConst::get(variant);
    m_geometry = &m_board_const->get_geometry();
    m_starting_points.init(variant, *m_geometry);
    m_point_state.init(*m_geometry);
    m_played_move.init(*m_geometry);
    // m_forbidden needs to be initialized even for colors not used in current
    // game variant because it is written to in some unrolled color loops
    LIBPENTOBI_FOREACH_COLOR(c, m_forbidden[c].init(*m_geometry));
    for (ColorIterator i(m_nu_colors); i; ++i)
    {
        if (variant == variant_classic_2
            || variant == variant_trigon_2)
            m_second_color[*i] = get_next(get_next(*i));
        else
            m_second_color[*i] = *i;
        m_is_attach_point[*i].init(*m_geometry);
    }
}

bool Board::is_game_over() const
{
    for (ColorIterator i(m_nu_colors); i; ++i)
        if (has_moves(*i))
            return false;
    return true;
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

void Board::write(ostream& out, bool mark_last_move) const
{
    ColorMove last_mv = ColorMove::null();
    if (mark_last_move)
    {
        unsigned int n = get_nu_moves();
        while (n > 0)
        {
            --n;
            last_mv = get_move(n);
            if (! last_mv.move.is_pass())
                break;
        }
    }
    unsigned int width = m_geometry->get_width();
    unsigned int height = m_geometry->get_height();
    bool is_info_location_right = (width <= 20);
    BoardType board_type = get_board_type();
    bool is_trigon = (board_type == board_type_trigon
                      || board_type == board_type_trigon_3);
    write_x_coord(out, width, is_trigon ? 3 : 2);
    for (unsigned int y = height - 1; ; --y)
    {
        if (y < 9)
            out << ' ';
        out << (y + 1) << ' ';
        for (unsigned int x = 0; x < width; ++x)
        {
            Point p(x, y);
            bool is_offboard = ! is_onboard(p);
            if ((x > 0 || (is_trigon && x == 0 && is_onboard(p.get_right())))
                && ! is_offboard)
            {
                // Print a space horizontally between fields on the board. On a
                // Trigon board, a slash or backslash is used instead of the
                // space to indicate the orientation of the triangles. A
                // less-than/greater-than character is used instead of the space
                // to mark the last piece played (the mark is not placed within
                // the piece or off-board).
                if (! last_mv.is_null()
                    && get_played_move(p) == last_mv.move
                    && (x == 0 || ! is_onboard(p.get_left())
                        || get_point_state(p.get_left()) != last_mv.color))
                {
                    set_color(out, "\x1B[1;37;47m");
                    out << '>';
                    last_mv = ColorMove::null();
                }
                else if (! last_mv.is_null()
                         && x > 0 && is_onboard(p.get_left())
                         && get_played_move(p.get_left()) == last_mv.move
                         && get_point_state(p) != last_mv.color
                         && get_point_state(p.get_left()) == last_mv.color)
                {
                    set_color(out, "\x1B[1;37;47m");
                    out << '<';
                    last_mv = ColorMove::null();
                }
                else if (is_trigon)
                {
                    set_color(out, "\x1B[1;30;47m");
                    out << (m_geometry->get_point_type(x, y) == 0 ? '\\' : '/');
                }
                else
                {
                    set_color(out, "\x1B[1;30;47m");
                    out << ' ';
                }
            }
            if (is_offboard)
            {
                if (is_trigon && x > 0 && is_onboard(p.get_left()))
                {
                    set_color(out, "\x1B[1;30;47m");
                    out << (m_geometry->get_point_type(p) == 0 ? '\\' : '/');
                }
                else
                {
                    set_color(out, "\x1B[0m");
                    out << "  ";
                }
            }
            else
            {
                PointState s = get_point_state(p);
                if (s.is_empty())
                {
                    if (is_colored_starting_point(p))
                    {
                        Color c = get_starting_point_color(p);
                        set_color(out, m_color_esc_sequence[c]);
                        out << '+';
                    }
                    else if (is_colorless_starting_point(p))
                    {
                        set_color(out, "\x1B[1;30;47m");
                        out << '+';
                    }
                    else
                    {
                        set_color(out, "\x1B[1;30;47m");
                        out << (is_trigon ? ' ' : '.');
                    }
                }
                else
                {
                    Color color = s.to_color();
                    set_color(out, m_color_esc_sequence[color]);
                    out << m_color_char[color];
                }
            }
        }
        if (is_trigon)
        {
            if (is_onboard(Point(width - 1, y)))
            {
                set_color(out, "\x1B[1;30;47m");
                out << (m_geometry->get_point_type(width - 1, y) != 0 ?
                        '\\' : '/');
            }
            else
            {
                set_color(out, "\x1B[0m");
                out << "  ";
            }
        }
        set_color(out, "\x1B[0m");
        out << ' ' << (y + 1);
        if (is_info_location_right)
        {
            if (y < 9)
                out << "   ";
            else
                out << "  ";
            write_info_line(out, height - y - 1);
        }
        out << '\n';
        if (y == 0)
            break;
    }
    write_x_coord(out, width, is_trigon ? 3 : 2);
    if (! is_info_location_right)
        for (ColorIterator i(m_nu_colors); i; ++i)
        {
            write_color_info_line1(out, *i);
            out << ' ';
            write_color_info_line2(out, *i);
            out << ' ';
            write_color_info_line3(out, *i);
            out << '\n';
        }
}

void Board::write_color_info_line1(ostream& out, Color c) const
{
    set_color(out, m_color_esc_sequence_text[c]);
    if (get_to_play() == c)
        out << '*';
    out << m_color_name[c] << "(" << m_color_char[c] << "): " << get_points(c);
    unsigned int bonus = get_bonus(c);
    if (bonus > 0)
        out << " (+" << bonus << ')';
    set_color(out, "\x1B[0m");
}

void Board::write_color_info_line2(ostream& out, Color c) const
{
    if (m_variant == variant_junior)
        write_pieces_left(out, c, 0, 6);
    else
        write_pieces_left(out, c, 0, 10);
}

void Board::write_color_info_line3(ostream& out, Color c) const
{
    if (m_variant == variant_junior)
        write_pieces_left(out, c, 6, get_nu_pieces());
    else
        write_pieces_left(out, c, 10, get_nu_pieces());
}

void Board::write_info_line(ostream& out, unsigned int y) const
{
    if (y == 0)
        write_color_info_line1(out, Color(0));
    else if (y == 1)
        write_color_info_line2(out, Color(0));
    else if (y == 2)
        write_color_info_line3(out, Color(0));
    else if (y == 4)
        write_color_info_line1(out, Color(1));
    else if (y == 5)
        write_color_info_line2(out, Color(1));
    else if (y == 6)
        write_color_info_line3(out, Color(1));
    else if (y == 8 && m_nu_colors > 2)
        write_color_info_line1(out, Color(2));
    else if (y == 9 && m_nu_colors > 2)
        write_color_info_line2(out, Color(2));
    else if (y == 10 && m_nu_colors > 2)
        write_color_info_line3(out, Color(2));
    else if (y == 12 && m_nu_colors > 3)
        write_color_info_line1(out, Color(3));
    else if (y == 13 && m_nu_colors > 3)
        write_color_info_line2(out, Color(3));
    else if (y == 14 && m_nu_colors > 3)
        write_color_info_line3(out, Color(3));
}

void Board::write_pieces_left(ostream& out, Color c, unsigned int begin,
                              unsigned int end) const
{
    for (unsigned int i = begin; i < end; ++i)
        if (i < m_pieces_left[c].size())
        {
            if (i > begin)
                out << ' ';
            Piece piece = m_pieces_left[c][i];
            const string& name = get_piece_info(piece).get_name();
            unsigned int nu_left = m_nu_left_piece[c][piece];
            for (unsigned int j = 0; j < nu_left; ++j)
            {
                if (j > 0)
                    out << ' ';
                out << name;
            }
        }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
