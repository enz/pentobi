//-----------------------------------------------------------------------------
/** @file BoardConst.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardConst.h"

#include "AdjIterator.h"
#include "DiagIterator.h"
#include "Grid.h"
#include "SymmetricPoints.h"
#include "libboardgame_base/RectGeometry.h"
#include "libboardgame_base/Transform.h"
#include "libboardgame_base/TrigonGeometry.h"
#include "libboardgame_util/Log.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_base::RectGeometry;
using libboardgame_base::Transform;
using libboardgame_base::TrigonGeometry;
using libboardgame_util::log;

//-----------------------------------------------------------------------------

namespace {

const bool log_move_creation = false;

vector<Piece> create_pieces(BoardType board_type)
{
    vector<Piece> pieces;
    if (board_type == board_type_trigon)
    {
        log() << "TODO: define pieces for Trigon\n";
        return pieces;
    }
    // Define the 21 standard pieces. The piece names are the standard names as
    // http://c2strategy.wordpress.com/2011/04/10/piece-names/. The default
    // orientation is chosen such that it resembles the letter in the piece name
    pieces.reserve(BoardConst::max_pieces);
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        pieces.push_back(Piece("X", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, -1));
        pieces.push_back(Piece("F", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        pieces.push_back(Piece("L5", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        pieces.push_back(Piece("N", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(Piece("P", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(Piece("T5", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(Piece("U", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(2, 0));
        pieces.push_back(Piece("V5", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(Piece("W", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, -2));
        pieces.push_back(Piece("Y", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(Piece("Z5", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, -2));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        pieces.push_back(Piece("I5", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(Piece("O", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(0, -1));
        pieces.push_back(Piece("T4", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(Piece("Z4", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(Piece("L4", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        pieces.push_back(Piece("I4", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        pieces.push_back(Piece("V3", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        pieces.push_back(Piece("I3", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        pieces.push_back(Piece("2", points));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, 0));
        pieces.push_back(Piece("1", points));
    }
    return pieces;
}

const Geometry& create_geometry(BoardType board_type)
{
    if (board_type == board_type_classic)
        return *RectGeometry<Point>::get(20, 20);
    else if (board_type == board_type_duo)
        return *RectGeometry<Point>::get(14, 14);
    else
    {
        LIBBOARDGAME_ASSERT(board_type == board_type_trigon);
        return *TrigonGeometry<Point>::get(9);
    }
}

} // namespace

//-----------------------------------------------------------------------------

BoardConst::BoardConst(BoardType board_type)
    : m_geometry(create_geometry(board_type))
{
    m_board_type = board_type;
    m_pieces = create_pieces(board_type);
    m_nu_pieces = m_pieces.size();
    for (int s0 = 0; s0 <= 1; ++s0)
        for (int s1 = 0; s1 <= 1; ++s1)
            for (int s2 = 0; s2 <= 1; ++s2)
                for (int s3 = 0; s3 <= 1; ++s3)
                {
                    unsigned int index =
                         get_adj_status_index(s0 != 0, s1 != 0,
                                              s2 != 0, s3 != 0);
                    if (s0 != 0)
                        m_adj_status[index].push_back(0);
                    if (s1 != 0)
                        m_adj_status[index].push_back(1);
                    if (s2 != 0)
                        m_adj_status[index].push_back(2);
                    if (s3 != 0)
                        m_adj_status[index].push_back(3);
                }
    for (unsigned int i = 0; i < m_pieces.size(); ++i)
        create_moves(i);
    if (log_move_creation)
        log() << "Created moves: " << m_move_info.size() << '\n';
    LIBBOARDGAME_ASSERT(m_move_info.size() <= Move::range - 2);
    LIBBOARDGAME_ASSERT(board_type != board_type_classic
                        || m_move_info.size() == Move::range - 2);
    m_total_piece_points = 0;
    BOOST_FOREACH(const Piece& piece, m_pieces)
        m_total_piece_points += piece.get_size();
    if (board_type == board_type_classic || board_type == board_type_duo)
    {
        LIBBOARDGAME_ASSERT(m_nu_pieces == 21);
        LIBBOARDGAME_ASSERT(m_total_piece_points == 89);
    }
    else if (board_type == board_type_trigon)
        log() << "TODO: assert nu pieces and total piece points in Trigon\n";
    init_symmetry_info();
}

void BoardConst::create_move(unsigned int piece,
                             const Piece::Points& coord_points,
                             CoordPoint center)
{
    MovePoints points;
    for (auto i = coord_points.begin(); i != coord_points.end(); ++i)
        points.push_back(Point((*i).x, (*i).y));
    MoveInfo info;
    info.piece = piece;
    info.points = points;
    info.center = Point(center.x, center.y);
    set_adj_and_corner_points(info);
    m_move_info.push_back(info);
    Move move(static_cast<unsigned int>(m_move_info.size() - 1));
    if (log_move_creation)
    {
        Grid<char> grid(m_geometry, '.');
        BOOST_FOREACH(Point p, info.points)
            grid[p] = 'O';
        BOOST_FOREACH(Point p, info.adj_points)
            grid[p] = '+';
        BOOST_FOREACH(Point p, info.attach_points)
            grid[p] = '*';
        log() << "Move " << move.to_int() << ":\n" << grid << '\n';
    }
    BOOST_FOREACH(Point p, points)
        for (unsigned int i = 0; i < 16; ++i)
        {
            if (is_compatible_with_adj_status(p, i, points))
                m_moves[i][piece][p].push_back(move);
        }
}

void BoardConst::create_moves(unsigned int piece)
{
    for (unsigned int i = 0; i < 16; ++i)
        m_moves[i][piece].init(m_geometry);
    Piece::Points points;
    for (GeometryIterator i(m_geometry); i; ++i)
    {
        unsigned int x = (*i).get_x();
        unsigned int y = (*i).get_y();
        BOOST_FOREACH(Transform transform, m_pieces[piece].get_transforms())
        {
            points = m_pieces[piece].get_points();
            transform.transform(points.begin(), points.end());
            sort(points.begin(), points.end());
            auto center = find(points.begin(), points.end(), CoordPoint(0, 0));
            LIBBOARDGAME_ASSERT(center != points.end());
            unsigned int width;
            unsigned int height;
            CoordPoint::normalize_offset(points.begin(), points.end(),
                                         width, height);
            bool is_onboard = true;
            BOOST_FOREACH(CoordPoint& p, points)
            {
                p.x += x;
                p.y += y;
                if (! m_geometry.is_onboard(p))
                {
                    is_onboard = false;
                    break;
                }
            }
            if (is_onboard)
                create_move(piece, points, *center);
        }
    }
}

const BoardConst& BoardConst::get(BoardType board_type)
{
    static unique_ptr<BoardConst> board_const_classic;
    static unique_ptr<BoardConst> board_const_duo;
    static unique_ptr<BoardConst> board_const_trigon;
    if (board_type == board_type_classic)
    {
        if (board_const_classic.get() == 0)
            board_const_classic.reset(new BoardConst(board_type_classic));
        return *board_const_classic;
    }
    else if (board_type == board_type_duo)
    {
        if (board_const_duo.get() == 0)
            board_const_duo.reset(new BoardConst(board_type_duo));
        return *board_const_duo;
    }
    else
    {
        if (board_const_trigon.get() == 0)
            board_const_trigon.reset(new BoardConst(board_type_trigon));
        return *board_const_trigon;
    }
}

bool BoardConst::get_piece_index_by_name(const string& name,
                                         unsigned int& index) const
{
    for (unsigned int i = 0; i < m_nu_pieces; ++i)
        if (get_piece(i).get_name() == name)
        {
            index = i;
            return true;
        }
    return false;
}

bool BoardConst::find_move(const MovePoints& points, Move& move) const
{
    if (points.size() == 0)
        return false;
    MovePoints sorted_points = points;
    sort(sorted_points.begin(), sorted_points.end());
    Point p = points[0];
    for (unsigned int i = 0; i < m_pieces.size(); ++i)
        if (get_piece(i).get_size() == points.size())
        {
            const vector<Move>& moves = get_moves(i, p);
            for (unsigned int j = 0; j < moves.size(); ++j)
                if (m_move_info[moves[j].to_int()].points == sorted_points)
                {
                    move = moves[j];
                    return true;
                }
        }
    return false;
}

void BoardConst::init_symmetry_info()
{
    SymmetricPoints symmetric_points;
    symmetric_points.init(m_geometry);
    BOOST_FOREACH(MoveInfo& info, m_move_info)
    {
        MovePoints sym_points;
        info.breaks_symmetry = false;
        BOOST_FOREACH(Point p, info.points)
        {
            if (info.points.contains(symmetric_points[p]))
                info.breaks_symmetry = true;
            sym_points.push_back(symmetric_points[p]);
        }
        find_move(sym_points, info.symmetric_move);
    }
}

bool BoardConst::is_compatible_with_adj_status(Point p,
                                               unsigned int adj_status_index,
                                               const MovePoints& points) const
{
    for (unsigned int i = 0; i < 4; ++i)
    {
        if (m_adj_status[adj_status_index].contains(i))
        {
            Point p_adj = p.get_neighbor(Direction::get_enum_adj(i));
            if (points.contains(p_adj))
                return false;
        }
    }
    return true;
}

void BoardConst::set_adj_and_corner_points(MoveInfo& info)
{
    auto begin = info.points.begin();
    auto end = info.points.end();
    m_marker.clear();
    for (auto i = begin; i != end; ++i)
        m_marker.set(*i);
    info.adj_points.clear();
    for (auto i = begin; i != end; ++i)
        for (AdjIterator j(m_geometry, *i); j; ++j)
            if (m_geometry.is_onboard(*j) && ! m_marker[*j])
            {
                m_marker.set(*j);
                info.adj_points.push_back(*j);
            }
    info.attach_points.clear();
    for (auto i = begin; i != end; ++i)
        for (DiagIterator j(m_geometry, *i); j; ++j)
            if (m_geometry.is_onboard(*j) && ! m_marker[*j])
            {
                m_marker.set(*j);
                info.attach_points.push_back(*j);
            }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
