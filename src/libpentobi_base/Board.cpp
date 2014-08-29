//-----------------------------------------------------------------------------
/** @file libpentobi_base/Board.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Board.h"

#include <functional>
#include "libboardgame_util/Unused.h"

namespace libpentobi_base {

using namespace std;

//-----------------------------------------------------------------------------

namespace {

void write_x_coord(ostream& out, unsigned width, unsigned offset)
{
    for (unsigned i = 0; i < offset; ++i)
        out << ' ';
    char c = 'A';
    for (unsigned x = 0; x < width; ++x, ++c)
    {
        if (x < 26)
            out << ' ';
        else
            out << 'A';
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
    m_moves = bd.m_moves;
    m_setup.to_play = bd.m_setup.to_play;
    m_state_base = bd.m_state_base;
    for (ColorIterator i(m_nu_colors); i; ++i)
    {
        m_state_color[*i] = bd.m_state_color[*i];
        m_setup.placements[*i] = bd.m_setup.placements[*i];
        m_attach_points[*i] = bd.m_attach_points[*i];
    }
}

void Board::gen_moves(Color c, ArrayList<Move, Move::range>& moves) const
{
    moves.clear();
    if (is_first_piece(c))
    {
        for (Point p : get_starting_points(c))
            if (! m_state_color[c].forbidden[p])
                gen_moves(c, p, m_marker, moves);
    }
    else
    {
        for (Iterator i(*this); i; ++i)
            if (is_attach_point(*i, c) && ! m_state_color[c].forbidden[*i])
                gen_moves(c, *i, get_adj_status(*i, c), m_marker, moves);
    }
    m_marker.clear_all_set_known(moves);
}

void Board::gen_moves(Color c, Point p, MoveMarker& marker,
                      ArrayList<Move, Move::range>& moves) const
{
    for (Piece piece : m_state_color[c].pieces_left)
        for (Move mv : m_board_const->get_moves(piece, p))
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

void Board::gen_moves(Color c, Point p, unsigned adj_status,
                      MoveMarker& marker,
                      ArrayList<Move,Move::range>& moves) const
{
    for (Piece piece : m_state_color[c].pieces_left)
        for (Move mv : m_board_const->get_moves(piece, p, adj_status))
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

Color Board::get_effective_to_play() const
{
    Color c = m_state_base.to_play;
    do
    {
        if (has_moves(c))
            return c;
        c = get_next(c);
    }
    while (c != m_state_base.to_play);
    return c;
}

void Board::get_place(Color c, unsigned& place, bool& is_shared) const
{
    array<int,Color::range> all_scores;
    for (Color::IntType i = 0; i < Color::range; ++i)
        all_scores[i] = get_score(Color(i));
    int score = all_scores[c.to_int()];
    sort(all_scores.begin(), all_scores.begin() + m_nu_players, greater<int>());
    is_shared = false;
    bool found = false;
    for (unsigned i = 0; i < m_nu_players; ++i)
        if (all_scores[i] == score)
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

Move Board::get_move_at(Point p) const
{
    auto s = get_point_state(p);
    if (s.is_color())
    {
        auto c = s.to_color();
        for (Move mv : m_setup.placements[c])
            if (get_move_info(mv).contains(p))
                return mv;
        for (ColorMove color_mv : m_moves)
            if (color_mv.color == c)
            {
                Move mv = color_mv.move;
                if (! mv.is_pass() && get_move_info(mv).contains(p))
                    return mv;
            }
    }
    return Move::null();
}

bool Board::has_moves(Color c) const
{
    bool is_first = is_first_piece(c);
    for (Iterator i(*this); i; ++i)
        if (! m_state_color[c].forbidden[*i]
            && (is_attach_point(*i, c)
                || (is_first && get_starting_points(c).contains(*i))))
            if (has_moves(c, *i))
                return true;
    return false;
}

bool Board::has_moves(Color c, Point p) const
{
    for (Piece piece : m_state_color[c].pieces_left)
        for (Move mv : m_board_const->get_moves(piece, p))
            if (! is_forbidden(c, mv))
                return true;
    return false;
}

bool Board::has_setup() const
{
    for (ColorIterator i(m_nu_colors); i; ++i)
        if (! m_setup.placements[*i].empty())
            return true;
    return false;
}

void Board::init(Variant variant, const Setup* setup)
{
    if (variant != m_variant)
        init_variant(variant);

    // If you make changes here, make sure that you also update copy_from()

    m_state_base.point_state.fill(PointState::empty(), *m_geo);
    if (variant == Variant::junior)
        m_nu_piece_instances = 2;
    else
        m_nu_piece_instances = 1;
    for (ColorIterator i(m_nu_colors); i; ++i)
    {
        m_state_color[*i].forbidden.fill(false, *m_geo);
        m_state_color[*i].is_attach_point.fill(false, *m_geo);
        m_attach_points[*i].clear();
        m_state_color[*i].pieces_left.clear();
        m_state_color[*i].nu_onboard_pieces = 0;
        m_state_color[*i].points = 0;
        for (Piece::IntType j = 0; j < get_nu_uniq_pieces(); ++j)
            m_state_color[*i].pieces_left.push_back(Piece(j));
        m_state_color[*i].nu_left_piece.fill(m_nu_piece_instances);
    }
    m_state_base.nu_onboard_pieces_all = 0;
    if (setup == nullptr)
    {
        m_setup.clear();
        m_state_base.to_play = Color(0);
    }
    else
    {
        m_setup = *setup;
        place_setup(m_setup);
        m_state_base.to_play = setup->to_play;
        optimize_attach_point_lists();
        for (ColorIterator i(m_nu_colors); i; ++i)
            if (m_state_color[*i].pieces_left.empty())
                m_state_color[*i].points += m_bonus_all_pieces;
    }
    m_moves.clear();
}

void Board::init_variant(Variant variant)
{
    m_variant = variant;
    m_nu_colors = libpentobi_base::get_nu_colors(variant);
    if (m_nu_colors == 2)
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
    m_nu_players = libpentobi_base::get_nu_players(variant);
    m_board_const = &BoardConst::get(variant);
    if (m_variant == Variant::junior)
    {
        m_bonus_all_pieces = 0;
        m_bonus_one_piece = 0;
    }
    else
    {
        m_bonus_all_pieces = 15;
        m_bonus_one_piece = 5;
    }
    m_geo = &m_board_const->get_geometry();
    m_move_info_array = m_board_const->get_move_info_array();
    m_move_info_ext_array = m_board_const->get_move_info_ext_array();
    m_move_info_ext_2_array = m_board_const->get_move_info_ext_2_array();
    m_starting_points.init(variant, *m_geo);
    for (ColorIterator i(m_nu_colors); i; ++i)
    {
        if (variant == Variant::classic_2
            || variant == Variant::trigon_2)
            m_second_color[*i] = get_next(get_next(*i));
        else
            m_second_color[*i] = *i;
    }
}

bool Board::is_game_over() const
{
    for (ColorIterator i(m_nu_colors); i; ++i)
        if (has_moves(*i))
            return false;
    return true;
}

/** Remove forbidden points from attach point lists.
    The attach point lists do not guarantee that they contain only
    non-forbidden attach points because that would be too expensive to
    incrementally update but at certain points that are not performance
    critical (e.g. before taking a snapshot), we can remove them. */
void Board::optimize_attach_point_lists()
{
    for (ColorIterator i(m_nu_colors); i; ++i)
    {
        auto& attach_points = m_attach_points[*i];
        for (auto j = attach_points.begin(); j != attach_points.end(); ++j)
            if (is_forbidden(*j, *i))
            {
                m_state_color[*i].is_attach_point[*j] = false;
                attach_points.remove_fast(j);
                --j;
                continue;
            }
    }
}

/** Place setup moves on board.
    This function is only extracted from init() because it contains the large
    inline function place(), which will cause a failure to inline and warning
    with -Winline with GCC 4.8.2. */
void Board::place_setup(const Setup& setup)
{
    for (ColorIterator i(m_nu_colors); i; ++i)
        for (Move mv : setup.placements[*i])
            place(*i, mv);
}

void Board::take_snapshot()
{
    if (! m_snapshot)
        m_snapshot.reset(new Snapshot());
    optimize_attach_point_lists();
    m_snapshot->moves_size = m_moves.size();
    m_snapshot->state_base.to_play = m_state_base.to_play;
    m_snapshot->state_base.nu_onboard_pieces_all =
        m_state_base.nu_onboard_pieces_all;
    m_snapshot->state_base.point_state.memcpy_from(m_state_base.point_state,
                                                   *m_geo);
    for (ColorIterator i(m_nu_colors); i; ++i)
    {
        m_snapshot->attach_points_size[*i] = m_attach_points[*i].size();
        const auto& state = m_state_color[*i];
        auto& snapshot_state = m_snapshot->state_color[*i];
        snapshot_state.forbidden.memcpy_from(state.forbidden, *m_geo);
        snapshot_state.is_attach_point.memcpy_from(state.is_attach_point,
                                                   *m_geo);
        // Uncomment one is_trivially_copyable is implemented in GCC and MSVC
        //static_assert(is_trivially_copyable<StateColor>::value, "")
        memcpy(&snapshot_state.pieces_left, &state.pieces_left,
               sizeof(StateColor) - offsetof(StateColor, pieces_left));
    }
}

void Board::undo()
{
    LIBBOARDGAME_ASSERT(get_nu_moves() > 0);
    ArrayList<ColorMove, max_game_moves> moves = m_moves;
    moves.pop_back();
    init();
    for (unsigned i = 0; i < moves.size(); ++i)
        play(moves[i]);
}

void Board::write(ostream& out, bool mark_last_move) const
{
    // Sort lists of left pieces by name
    ColorMap<PiecesLeftList> pieces_left;
    for (ColorIterator i(m_nu_colors); i; ++i)
    {
        pieces_left[*i] = m_state_color[*i].pieces_left;
        sort(pieces_left[*i].begin(), pieces_left[*i].end(),
             [&](Piece p1, Piece p2)
             {
                 return
                     get_piece_info(p1).get_name()
                         < get_piece_info(p2).get_name();
             });
    }

    ColorMove last_mv = ColorMove::null();
    if (mark_last_move)
    {
        unsigned n = get_nu_moves();
        while (n > 0)
        {
            --n;
            last_mv = get_move(n);
            if (! last_mv.move.is_pass())
                break;
        }
    }
    unsigned width = m_geo->get_width();
    unsigned height = m_geo->get_height();
    bool is_info_location_right = (width <= 20);
    auto board_type = get_board_type();
    bool is_trigon = (board_type == BoardType::trigon
                      || board_type == BoardType::trigon_3);
    write_x_coord(out, width, is_trigon ? 3 : 2);
    for (unsigned y = 0; y < height; ++y)
    {
        if (height - y < 10)
            out << ' ';
        out << (height - y) << ' ';
        for (unsigned x = 0; x < width; ++x)
        {
            Point p(x, y, width);
            bool is_offboard = ! is_onboard(p);
            if ((x > 0 || (is_trigon && x == 0 && is_onboard(p.get_right())))
                && ! is_offboard)
            {
                // Print a space horizontally between fields on the board. On a
                // Trigon board, a slash or backslash is used instead of the
                // space to indicate the orientation of the triangles. A
                // less-than/greater-than character is used instead of the
                // space to mark the last piece played (the mark is not placed
                // within the piece or off-board).
                if (! last_mv.is_null()
                    && get_move_info(last_mv.move).contains(p)
                    && (x == 0 || ! is_onboard(p.get_left())
                        || get_point_state(p.get_left()) != last_mv.color))
                {
                    set_color(out, "\x1B[1;37;47m");
                    out << '>';
                    last_mv = ColorMove::null();
                }
                else if (! last_mv.is_null()
                         && x > 0 && is_onboard(p.get_left())
                         && get_move_info(last_mv.move).contains(p.get_left())
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
                    out << (m_geo->get_point_type(x, y) == 0 ? '\\' : '/');
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
                    out << (m_geo->get_point_type(p) == 0 ? '\\' : '/');
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
            if (is_onboard(Point(width - 1, y, width)))
            {
                set_color(out, "\x1B[1;30;47m");
                out << (m_geo->get_point_type(width - 1, y) != 0 ? '\\' : '/');
            }
            else
            {
                set_color(out, "\x1B[0m");
                out << "  ";
            }
        }
        set_color(out, "\x1B[0m");
        out << ' ' << (height - y);
        if (is_info_location_right)
        {
            if (height - y < 10)
                out << "   ";
            else
                out << "  ";
            write_info_line(out, y, pieces_left);
        }
        out << '\n';
    }
    write_x_coord(out, width, is_trigon ? 3 : 2);
    if (! is_info_location_right)
        for (ColorIterator i(m_nu_colors); i; ++i)
        {
            write_color_info_line1(out, *i);
            out << ' ';
            write_color_info_line2(out, *i, pieces_left[*i]);
            out << ' ';
            write_color_info_line3(out, *i, pieces_left[*i]);
            out << '\n';
        }
}

void Board::write_color_info_line1(ostream& out, Color c) const
{
    set_color(out, m_color_esc_sequence_text[c]);
    if (get_to_play() == c)
        out << '*';
    out << m_color_name[c] << "(" << m_color_char[c] << "): " << get_points(c);
    set_color(out, "\x1B[0m");
}

void Board::write_color_info_line2(ostream& out, Color c,
                                   const PiecesLeftList& pieces_left) const
{
    if (m_variant == Variant::junior)
        write_pieces_left(out, c, pieces_left, 0, 6);
    else
        write_pieces_left(out, c, pieces_left, 0, 10);
}

void Board::write_color_info_line3(ostream& out, Color c,
                                   const PiecesLeftList& pieces_left) const
{
    if (m_variant == Variant::junior)
        write_pieces_left(out, c, pieces_left, 6, get_nu_uniq_pieces());
    else
        write_pieces_left(out, c, pieces_left, 10, get_nu_uniq_pieces());
}

void Board::write_info_line(ostream& out, unsigned y,
                            const ColorMap<PiecesLeftList>& pieces_left) const
{
    if (y == 0)
        write_color_info_line1(out, Color(0));
    else if (y == 1)
        write_color_info_line2(out, Color(0), pieces_left[Color(0)]);
    else if (y == 2)
        write_color_info_line3(out, Color(0), pieces_left[Color(0)]);
    else if (y == 4)
        write_color_info_line1(out, Color(1));
    else if (y == 5)
        write_color_info_line2(out, Color(1), pieces_left[Color(1)]);
    else if (y == 6)
        write_color_info_line3(out, Color(1), pieces_left[Color(1)]);
    else if (y == 8 && m_nu_colors > 2)
        write_color_info_line1(out, Color(2));
    else if (y == 9 && m_nu_colors > 2)
        write_color_info_line2(out, Color(2), pieces_left[Color(2)]);
    else if (y == 10 && m_nu_colors > 2)
        write_color_info_line3(out, Color(2), pieces_left[Color(2)]);
    else if (y == 12 && m_nu_colors > 3)
        write_color_info_line1(out, Color(3));
    else if (y == 13 && m_nu_colors > 3)
        write_color_info_line2(out, Color(3), pieces_left[Color(3)]);
    else if (y == 14 && m_nu_colors > 3)
        write_color_info_line3(out, Color(3), pieces_left[Color(3)]);
}

void Board::write_pieces_left(ostream& out, Color c,
                              const PiecesLeftList& pieces_left,
                              unsigned begin, unsigned end) const
{
    for (unsigned i = begin; i < end; ++i)
        if (i < pieces_left.size())
        {
            if (i > begin)
                out << ' ';
            Piece piece = pieces_left[i];
            auto& name = get_piece_info(piece).get_name();
            unsigned nu_left = m_state_color[c].nu_left_piece[piece];
            for (unsigned j = 0; j < nu_left; ++j)
            {
                if (j > 0)
                    out << ' ';
                out << name;
            }
        }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
