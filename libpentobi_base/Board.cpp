//-----------------------------------------------------------------------------
/** @file libpentobi_base/Board.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Board.h"

#include "CallistoGeometry.h"
#include "MoveMarker.h"

namespace libpentobi_base {

//-----------------------------------------------------------------------------

namespace {

void write_x_coord(ostream& out, unsigned width, unsigned offset,
                   bool is_gembloq)
{
    for (unsigned i = 0; i < offset; ++i)
        out << ' ';
    if (is_gembloq)
    {
        char c = 'A';
        char c1 = ' ';
        out << ' ';
        for (unsigned x = 0; x < width; ++x, ++c)
        {
            if (x % 2 != 0)
                out << c1;
            if (x > 0 && x % 26 == 0)
            {
                c = 'A';
                if (c1 == ' ')
                    c1 = 'A';
                else
                    ++c1;
            }
            out << c;
        }
    }
    else
    {
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

Board::Board(Variant variant)
{
    m_color_char[Color(0)] = 'X';
    m_color_char[Color(1)] = 'O';
    m_color_char[Color(2)] = '#';
    m_color_char[Color(3)] = '@';
    for_each_color([&](Color c) {
        m_state_color[c].forbidden[Point::null()] = false;
    });
    init_variant(variant);
    init();
#ifdef LIBBOARDGAME_DEBUG
    m_snapshot.moves_size =
            numeric_limits<decltype(m_snapshot.moves_size)>::max();
#endif
}

void Board::copy_from(const Board& bd)
{
    if (m_variant != bd.m_variant)
        init_variant(bd.m_variant);
    m_moves = bd.m_moves;
    m_setup.to_play = bd.m_setup.to_play;
    m_state_base = bd.m_state_base;
    for (Color c : get_colors())
    {
        m_state_color[c] = bd.m_state_color[c];
        m_setup.placements[c] = bd.m_setup.placements[c];
        m_attach_points[c] = bd.m_attach_points[c];
    }
}

const Transform* Board::find_transform(Move mv) const
{
    auto& geo = get_geometry();
    PiecePoints points;
    for (Point p : get_move_points(mv))
        points.push_back({geo.get_x(p), geo.get_y(p)});
    return get_piece_info(get_move_piece(mv)).find_transform(geo, points);
}

void Board::gen_moves(Color c, MoveMarker& marker, MoveList& moves) const
{
    moves.clear();
    if (! m_is_callisto && is_first_piece(c))
    {
        for (Point p : get_starting_points(c))
            if (! m_state_color[c].forbidden[p])
            {
                auto adj_status = get_adj_status(p, c);
                for (Piece piece : m_state_color[c].pieces_left)
                    gen_moves(c, p, piece, adj_status, marker, moves);
            }
        return;
    }
    if (m_is_callisto && is_piece_left(c, m_one_piece))
        for (auto p : *m_geo)
            if (! is_forbidden(p, c) && ! m_is_center_section[p])
                gen_moves(c, p, m_one_piece, get_adj_status(p, c), marker,
                          moves);
    for (Point p : get_attach_points(c))
        if (! m_state_color[c].forbidden[p])
        {
            auto adj_status = get_adj_status(p, c);
            for (Piece piece : m_state_color[c].pieces_left)
                if (! m_is_callisto || piece != m_one_piece)
                    gen_moves(c, p, piece, adj_status, marker, moves);
        }
}

void Board::gen_moves(Color c, Point p, Piece piece, unsigned adj_status,
                      MoveMarker& marker, MoveList& moves) const
{
    for (Move mv : m_bc->get_moves(piece, p, adj_status))
        if (! marker[mv] && ! is_forbidden(c, mv))
        {
            moves.push_back(mv);
            marker.set(mv);
        }
}

ScoreType Board::get_bonus(Color c) const
{
    if (! get_pieces_left(c).empty())
        return 0;
    auto bonus = m_bonus_all_pieces;
    unsigned i = m_moves.size();
    while (i > 0)
    {
        --i;
        if (m_moves[i].color == c)
        {
            auto piece = get_move_piece(m_moves[i].move);
            if (m_score_points[piece] == 1)
                bonus += m_bonus_one_piece;
            break;
        }
    }
    return bonus;
}

Color Board::get_effective_to_play() const
{
    return get_effective_to_play(get_to_play());
}

Color Board::get_effective_to_play(Color c) const
{
    Color result = c;
    do
    {
        if (has_moves(result))
            return result;
        result = get_next(result);
    }
    while (result != c);
    return result;
}

void Board::get_place(Color c, unsigned& place, bool& is_shared) const
{
    bool break_ties = get_break_ties();
    array<ScoreType, Color::range> all_scores;
    for (Color::IntType i = 0; i < Color::range; ++i)
    {
        all_scores[i] = get_score(Color(i));
        if (break_ties)
            all_scores[i] += i * 0.0001f;
    }
    auto score = all_scores[c.to_int()];
    // Disable false-positive GCC warning if compiled with -O2
    // (last tested with GCC 13.2.0)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
    sort(all_scores.begin(), all_scores.begin() + m_nu_players, greater<>());
#pragma GCC diagnostic pop
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
            if (get_move_points(mv).contains(p))
                return mv;
        for (ColorMove color_mv : m_moves)
            if (color_mv.color == c)
            {
                Move mv = color_mv.move;
                if (get_move_points(mv).contains(p))
                    return mv;
            }
    }
    return Move::null();
}

bool Board::has_moves(Color c) const
{
    if (m_is_callisto && is_piece_left(c, m_one_piece))
        for (auto p : *m_geo)
            if (! is_forbidden(p, c) && ! m_is_center_section[p])
                return true;
    if (! m_is_callisto && is_first_piece(c))
    {
        for (auto p : get_starting_points(c))
            if (has_moves(c, p))
                return true;
        return false;
    }
    for (auto p : get_attach_points(c))
        if (has_moves(c, p))
            return true;
    return false;
}

bool Board::has_moves(Color c, Point p) const
{
    if (is_forbidden(p, c))
        return false;
    if (m_is_callisto && is_piece_left(c, m_one_piece))
        if (m_is_center_section[p])
            return true;
    auto adj_status = get_adj_status(p, c);
    for (auto piece : m_state_color[c].pieces_left)
    {
        if (piece == m_one_piece && m_is_callisto)
            continue;
        for (auto mv : m_bc->get_moves(piece, p, adj_status))
            if (! is_forbidden(c, mv))
                return true;
    }
    return false;
}

bool Board::has_setup() const
{
    for (Color c : get_colors())
        if (! m_setup.placements[c].empty())
            return true;
    return false;
}

void Board::init(Variant variant, const Setup* setup)
{
    if (variant != m_variant)
        init_variant(variant);

    // If you make changes here, make sure that you also update copy_from()

    m_state_base.point_state.fill(PointState::empty(), *m_geo);
    for (Color c : get_colors())
    {
        auto& state = m_state_color[c];
        state.forbidden.fill(false, *m_geo);
        state.is_attach_point.fill(false, *m_geo);
        state.pieces_left.clear();
        state.nu_onboard_pieces = 0;
        state.points = 0;
        for (Piece::IntType i = 0; i < get_nu_uniq_pieces(); ++i)
        {
            Piece piece(i);
            state.pieces_left.push_back(piece);
            state.nu_left_piece[piece] =
                    static_cast<uint_fast8_t>(get_nu_piece_instances(piece));
        }
        m_attach_points[c].clear();
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
        for (Color c : get_colors())
            if (m_state_color[c].pieces_left.empty())
                m_state_color[c].points += m_bonus_all_pieces;
    }
    m_moves.clear();
}

void Board::init_variant(Variant variant)
{
    m_variant = variant;
    m_nu_colors = libpentobi_base::get_nu_colors(variant);
    if (variant == Variant::duo)
    {
        m_color_name[Color(0)] = "Purple";
        m_color_name[Color(1)] = "Orange";
        m_color_esc_sequence[Color(0)] = "\x1B[1;35;47m";
        m_color_esc_sequence[Color(1)] = "\x1B[1;33;47m";
        m_color_esc_sequence_text[Color(0)] = "\x1B[1;35m";
        m_color_esc_sequence_text[Color(1)] = "\x1B[1;33m";
    }
    else if (variant == Variant::junior)
    {
        m_color_name[Color(0)] = "Green";
        m_color_name[Color(1)] = "Orange";
        m_color_esc_sequence[Color(0)] = "\x1B[1;32;47m";
        m_color_esc_sequence[Color(1)] = "\x1B[1;33;47m";
        m_color_esc_sequence_text[Color(0)] = "\x1B[1;32m";
        m_color_esc_sequence_text[Color(1)] = "\x1B[1;33m";
    }
    else if (m_nu_colors == 2)
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
    m_bc = &BoardConst::get(variant);
    m_piece_set = m_bc->get_piece_set();
    m_geometry_type = libpentobi_base::get_geometry_type(variant);
    m_is_callisto = (m_geometry_type == GeometryType::callisto);
    if ((m_piece_set == PieceSet::classic && variant != Variant::junior)
            || m_piece_set == PieceSet::trigon)
    {
        m_bonus_all_pieces = 15;
        m_bonus_one_piece = 5;
    }
    else if (m_piece_set == PieceSet::nexos)
    {
        m_bonus_all_pieces = 10;
        m_bonus_one_piece = 0;
    }
    else
    {
        m_bonus_all_pieces = 0;
        m_bonus_one_piece = 0;
    }
    m_max_piece_size = m_bc->get_max_piece_size();
    m_max_adj_attach = m_bc->get_max_adj_attach();
    m_geo = &m_bc->get_geometry();
    m_move_info_array = m_bc->get_move_info_array();
    m_move_info_ext_array = m_bc->get_move_info_ext_array();
    m_move_info_ext_2_array = m_bc->get_move_info_ext_2_array();
    m_starting_points.init(variant, *m_geo);
    if (m_piece_set == PieceSet::gembloq)
        m_needed_starting_points = 4;
    else
        m_needed_starting_points = 1;
    if (m_is_callisto)
        for (Point p : *m_geo)
            m_is_center_section[p] =
                    CallistoGeometry::is_center_section(m_geo->get_x(p),
                                                        m_geo->get_y(p),
                                                        m_nu_colors);
    else
        m_is_center_section.fill(false, *m_geo);
    for (Color c : get_colors())
    {
        if (m_nu_players == 2 && m_nu_colors == 4)
            m_second_color[c] = get_next(get_next(c));
        else
            m_second_color[c] = c;
    }
    for (Piece::IntType i = 0; i < get_nu_uniq_pieces(); ++i)
    {
        Piece piece(i);
        auto& piece_info = get_piece_info(piece);
        m_score_points[piece] = piece_info.get_score_points();
        if (piece_info.get_name() == "1")
            m_one_piece = piece;
    }
}

bool Board::is_game_over() const
{
    for (Color c : get_colors())
        if (has_moves(c))
            return false;
    return true;
}

bool Board::is_legal(Color c, Move mv) const
{
    auto piece = get_move_piece(mv);
    if (! is_piece_left(c, piece))
        return false;
    auto points = get_move_points(mv);
    auto i = points.begin();
    auto end = points.end();
    bool has_attach_point = false;
    do
    {
        if (m_state_color[c].forbidden[*i])
            return false;
        has_attach_point |= static_cast<int>(is_attach_point(*i, c));
    }
    while (++i != end);
    if (m_is_callisto)
    {
        if (m_state_color[c].nu_left_piece[m_one_piece] > 1
                && piece != m_one_piece)
            return false;
        if (piece == m_one_piece)
            return ! m_is_center_section[*points.begin()];
    }
    if (has_attach_point)
        return true;
    if (! is_first_piece(c))
        return false;
    i = points.begin();
    unsigned n = 0;
    do
        if (is_colorless_starting_point(*i)
                || (is_colored_starting_point(*i)
                    && get_starting_point_color(*i) == c))
            if (++n >= m_needed_starting_points)
                return true;
    while (++i != end);
    return false;
}

/** Remove forbidden points from attach point lists.
    The attach point lists do not guarantee that they contain only
    non-forbidden attach points because that would be too expensive to
    update incrementally but at certain times that are not performance
    critical (e.g. before taking a snapshot), we can remove them. */
void Board::optimize_attach_point_lists()
{
    PointList l;
    for (Color c : get_colors())
    {
        l.clear();
        for (Point p : m_attach_points[c])
            if (! is_forbidden(p, c))
                l.push_back(p);
        m_attach_points[c] = l;
    }
}

/** Place setup moves on board. */
void Board::place_setup(const Setup& setup)
{
    if (m_max_piece_size == 5)
        for (Color c : get_colors())
            for (Move mv : setup.placements[c])
                place<5, 16>(c, mv);
    else if (m_max_piece_size == 6)
        for (Color c : get_colors())
            for (Move mv : setup.placements[c])
                place<6, 22>(c, mv);
    else if (m_max_piece_size == 7)
        for (Color c : get_colors())
            for (Move mv : setup.placements[c])
                place<7, 12>(c, mv);
    else
        for (Color c : get_colors())
            for (Move mv : setup.placements[c])
                place<22, 44>(c, mv);
}

void Board::play(Color c, Move mv)
{
    if (m_max_piece_size == 5)
        play<5, 16>(c, mv);
    else if (m_max_piece_size == 6)
        play<6, 22>(c, mv);
    else if (m_max_piece_size == 7)
        play<7, 12>(c, mv);
    else
        play<22, 44>(c, mv);
}

void Board::take_snapshot()
{
    optimize_attach_point_lists();
    m_snapshot.moves_size = m_moves.size();
    m_snapshot.state_base.to_play = m_state_base.to_play;
    m_snapshot.state_base.nu_onboard_pieces_all =
        m_state_base.nu_onboard_pieces_all;
    m_snapshot.state_base.point_state.copy_from(m_state_base.point_state,
                                                *m_geo);
    for (Color c : get_colors())
    {
        m_snapshot.attach_points_size[c] = m_attach_points[c].size();
        const auto& state = m_state_color[c];
        auto& snapshot_state = m_snapshot.state_color[c];
        snapshot_state.forbidden.copy_from(state.forbidden, *m_geo);
        snapshot_state.is_attach_point.copy_from(state.is_attach_point,
                                                 *m_geo);
        snapshot_state.pieces_left = state.pieces_left;
        snapshot_state.nu_left_piece = state.nu_left_piece;
        snapshot_state.nu_onboard_pieces = state.nu_onboard_pieces;
        snapshot_state.points = state.points;
    }
}

void Board::write(ostream& out, bool mark_last_move) const
{
    // Sort lists of left pieces by name
    ColorMap<PiecesLeftList> pieces_left;
    for (Color c : get_colors())
    {
        pieces_left[c] = m_state_color[c].pieces_left;
        sort(pieces_left[c].begin(), pieces_left[c].end(),
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
        if (n > 0)
            last_mv = get_move(n - 1);
    }
    auto width = m_geo->get_width();
    auto height = m_geo->get_height();
    bool is_info_location_right = (width <= 20);
    bool is_trigon = (m_piece_set == PieceSet::trigon);
    bool is_nexos = (m_piece_set == PieceSet::nexos);
    bool is_gembloq = (m_piece_set == PieceSet::gembloq);
    for (unsigned y = 0; y < height; ++y)
    {
        if (height - y < 10)
            out << ' ';
        out << (height - y) << ' ';
        for (unsigned x = 0; x < width; ++x)
        {
            Point p = m_geo->get_point(x, y);
            bool is_offboard = p.is_null();
            auto point_type = m_geo->get_point_type(static_cast<int>(x),
                                                    static_cast<int>(y));
            if ((x > 0 || (is_trigon && x == 0 && m_geo->is_onboard(x + 1, y)))
                    && ! is_offboard)
            {
                // Print a space horizontally between fields on the board. On a
                // Trigon board, a slash or backslash is used instead of the
                // space to indicate the orientation of the triangles. A
                // less-than/greater-than character is used instead of the
                // space to mark the last piece played.
                if (! last_mv.is_null()
                        && get_move_points(last_mv.move).contains(p)
                        && (! m_geo->is_onboard(x - 1, y)
                            || get_point_state(m_geo->get_point(x - 1, y))
                               != last_mv.color))
                {
                    set_color(out, "\x1B[1;37;47m");
                    out << '>';
                    last_mv = ColorMove::null();
                }
                else if (! last_mv.is_null()
                         && m_geo->is_onboard(x - 1, y)
                         && get_move_points(last_mv.move).contains(
                                                   m_geo->get_point(x - 1, y))
                         && get_point_state(p) != last_mv.color
                         && get_point_state(m_geo->get_point(x - 1, y))
                                == last_mv.color)
                {
                    set_color(out, "\x1B[1;37;47m");
                    out << '<';
                    last_mv = ColorMove::null();
                }
                else if (is_trigon)
                {
                    set_color(out, "\x1B[1;30;47m");
                    out << (point_type == 1 ? '\\' : '/');
                }
                else if (is_gembloq)
                {
                    set_color(out, "\x1B[1;30;47m");
                    if (point_type == 1)
                        out << '/';
                    else if (point_type == 3)
                        out << '\\';
                }
                else
                {
                    set_color(out, "\x1B[1;30;47m");
                    out << ' ';
                }
            }
            if (is_offboard)
            {
                if (is_trigon && m_geo->is_onboard(x - 1, y))
                {
                    set_color(out, "\x1B[1;30;47m");
                    out << (point_type == 1 ? '\\' : '/');
                }
                else if (is_gembloq && m_geo->is_onboard(x - 1, y))
                {
                    set_color(out, "\x1B[1;30;47m");
                    if (point_type == 1)
                        out << '/';
                    else if (point_type == 3)
                        out << '\\';
                }
                else if (m_is_callisto && x == 0)
                {
                    set_color(out, "\x1B[0m");
                    out << ' ';
                }
                else if (is_gembloq)
                {
                    set_color(out, "\x1B[0m");
                    if (point_type == 1 || point_type == 3)
                        out << "  ";
                    else
                        out << ' ';
                }
                else
                {
                    set_color(out, is_nexos ? "\x1B[1;30;47m" : "\x1B[0m");
                    out << "  ";
                }
            }
            else
            {
                PointState s = get_point_state(p);
                if (s.is_empty())
                {
                    if (is_colored_starting_point(p) && ! is_nexos)
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
                        if (is_trigon || is_gembloq)
                            out << ' ';
                        else if (is_nexos && point_type == 1)
                            out << '-';
                        else if (is_nexos && point_type == 2)
                            out << '|';
                        else if (is_nexos && point_type == 0)
                            out << '+';
                        else if (m_is_callisto && is_center_section(p))
                            out << ',';
                        else
                            out << '.';
                    }
                }
                else
                {
                    Color color = s.to_color();
                    set_color(out, m_color_esc_sequence[color]);
                    if (is_nexos && m_geo->get_point_type(p) == 0)
                        out << '*'; // Uncrossable junction
                    else
                        out << m_color_char[color];
                }
            }
        }
        if (is_trigon)
        {
            if (m_geo->is_onboard(width - 1, y))
            {
                set_color(out, "\x1B[1;30;47m");
                out << (m_geo->get_point_type(static_cast<int>(width - 1),
                                              static_cast<int>(y)) != 1 ?
                            '\\' : '/');
            }
            else
            {
                set_color(out, "\x1B[0m");
                out << "  ";
            }
        }
        set_color(out, "\x1B[0m");
        if (is_info_location_right)
            write_info_line(out, y, pieces_left);
        out << '\n';
    }
    write_x_coord(out, width, is_trigon ? 3 : 2, is_gembloq);
    if (! is_info_location_right)
        for (Color c : get_colors())
        {
            write_color_info_line1(out, c);
            out << "  ";
            write_color_info_line2(out, c, pieces_left[c]);
            out << ' ';
            write_color_info_line3(out, c, pieces_left[c]);
            out << '\n';
        }
}

void Board::write_color_info_line1(ostream& out, Color c) const
{
    set_color(out, m_color_esc_sequence_text[c]);
    if (! is_game_over() && get_effective_to_play() == c)
        out << '(' << (get_nu_moves() + 1) << ") ";
    out << m_color_name[c] << "(" << m_color_char[c] << "): " << get_points(c);
    if (! has_moves(c))
        out << '!';
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
    {
        out << "  ";
        write_color_info_line1(out, Color(0));
    }
    else if (y == 1)
    {
        out << "  ";
        write_color_info_line2(out, Color(0), pieces_left[Color(0)]);
    }
    else if (y == 2)
    {
        out << "  ";
        write_color_info_line3(out, Color(0), pieces_left[Color(0)]);
    }
    else if (y == 4)
    {
        out << "  ";
        write_color_info_line1(out, Color(1));
    }
    else if (y == 5)
    {
        out << "  ";
        write_color_info_line2(out, Color(1), pieces_left[Color(1)]);
    }
    else if (y == 6)
    {
        out << "  ";
        write_color_info_line3(out, Color(1), pieces_left[Color(1)]);
    }
    else if (y == 8 && m_nu_colors > 2)
    {
        out << "  ";
        write_color_info_line1(out, Color(2));
    }
    else if (y == 9 && m_nu_colors > 2)
    {
        out << "  ";
        write_color_info_line2(out, Color(2), pieces_left[Color(2)]);
    }
    else if (y == 10 && m_nu_colors > 2)
    {
        out << "  ";
        write_color_info_line3(out, Color(2), pieces_left[Color(2)]);
    }
    else if (y == 12 && m_nu_colors > 3)
    {
        out << "  ";
        write_color_info_line1(out, Color(3));
    }
    else if (y == 13 && m_nu_colors > 3)
    {
        out << "  ";
        write_color_info_line2(out, Color(3), pieces_left[Color(3)]);
    }
    else if (y == 14 && m_nu_colors > 3)
    {
        out << "  ";
        write_color_info_line3(out, Color(3), pieces_left[Color(3)]);
    }
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
