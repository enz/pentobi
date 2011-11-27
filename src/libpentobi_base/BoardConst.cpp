//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardConst.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardConst.h"

#include "AdjIterator.h"
#include "AdjDiagIterator.h"
#include "DiagIterator.h"
#include "Grid.h"
#include "PieceTransformsClassic.h"
#include "PieceTransformsTrigon.h"
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

vector<Piece> create_pieces_classic(BoardType board_type,
                                    const Geometry& geometry,
                                    const PieceTransforms& transforms)
{
    vector<Piece> pieces;
    // Define the 21 standard pieces. The piece names are the standard names as
    // in http://blokusstrategy.com/?p=48. The default orientation is chosen
    // such that it resembles the letter in the piece name
    pieces.reserve(21);
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        pieces.push_back(Piece("X", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, -1));
        pieces.push_back(Piece("F", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        pieces.push_back(Piece("L5", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        pieces.push_back(Piece("N", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(Piece("P", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(Piece("T5", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(Piece("U", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(2, 0));
        pieces.push_back(Piece("V5", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(Piece("W", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, -2));
        pieces.push_back(Piece("Y", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(Piece("Z5", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, -2));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        pieces.push_back(Piece("I5", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(Piece("O", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(0, -1));
        pieces.push_back(Piece("T4", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(Piece("Z4", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(Piece("L4", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        pieces.push_back(Piece("I4", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        pieces.push_back(Piece("V3", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        pieces.push_back(Piece("I3", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        pieces.push_back(Piece("2", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, 0));
        pieces.push_back(Piece("1", points, geometry, transforms));
    }
    return pieces;
}

vector<Piece> create_pieces_trigon(BoardType board_type,
                                   const Geometry& geometry,
                                   const PieceTransforms& transforms)
{
    vector<Piece> pieces;
    // Define the 22 standard Trigon pieces. The piece names are similar to one
    // of the possible notations from the thread "Trigon book: how to play, how
    // to win" from August 2010 in the Blokus forums
    // http://forum.blokus.refreshed.be/viewtopic.php?f=2&t=2539#p9867
    // apart from that the smallest pieces are named '2' and '1' like in Classic
    // to avoid to many pieces with letter 'I' and that numbers are only used
    // if there is more than one piece with the same letter. The default
    // orientation is chosen such that it resembles the letter in the piece
    // name. The (0,0) point must have point type 0 (downside triangle).
    pieces.reserve(22);
    {
        Piece::Points points;
        points.push_back(CoordPoint(-2, -1));
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(Piece("I6", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(Piece("L6", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(-2, 0));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(2, 0));
        pieces.push_back(Piece("W", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-2, -1));
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(0, 1));
        pieces.push_back(Piece("P6", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(Piece("S", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        points.push_back(CoordPoint(2, 1));
        pieces.push_back(Piece("F", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(2, 0));
        points.push_back(CoordPoint(3, 0));
        pieces.push_back(Piece("V", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        points.push_back(CoordPoint(-1, 2));
        pieces.push_back(Piece("Y", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(Piece("A6", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        pieces.push_back(Piece("G", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(Piece("O", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(Piece("X", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        points.push_back(CoordPoint(1, 2));
        pieces.push_back(Piece("I5", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(0, 1));
        pieces.push_back(Piece("P5", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        pieces.push_back(Piece("L5", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        pieces.push_back(Piece("C5", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(Piece("I4", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(Piece("C4", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        pieces.push_back(Piece("A4", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        pieces.push_back(Piece("I3", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        pieces.push_back(Piece("2", points, geometry, transforms));
    }
    {
        Piece::Points points;
        points.push_back(CoordPoint(0, 0));
        pieces.push_back(Piece("1", points, geometry, transforms));
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
    if (board_type == board_type_trigon)
    {
        m_transforms.reset(new PieceTransformsTrigon());
        m_pieces = create_pieces_trigon(board_type, m_geometry, *m_transforms);
        m_move_info.reserve(Move::onboard_moves_trigon);
    }
    else if (board_type == board_type_classic)
    {
        m_transforms.reset(new PieceTransformsClassic());
        m_pieces = create_pieces_classic(board_type, m_geometry, *m_transforms);
        m_move_info.reserve(Move::onboard_moves_classic);
    }
    else
    {
        LIBBOARDGAME_ASSERT(board_type == board_type_duo);
        m_transforms.reset(new PieceTransformsClassic());
        m_pieces = create_pieces_classic(board_type, m_geometry, *m_transforms);
        m_move_info.reserve(Move::onboard_moves_duo);
    }
    m_nu_pieces = m_pieces.size();
    init_adj_status();
    for (unsigned int i = 0; i < m_pieces.size(); ++i)
        create_moves(i);
    if (log_move_creation)
        log() << "Created moves: " << m_move_info.size() << '\n';
    if (board_type == board_type_classic)
        LIBBOARDGAME_ASSERT(m_move_info.size() == Move::onboard_moves_classic);
    else if (board_type == board_type_duo)
        LIBBOARDGAME_ASSERT(m_move_info.size() == Move::onboard_moves_duo);
    else if (board_type == board_type_trigon)
        LIBBOARDGAME_ASSERT(m_move_info.size() == Move::onboard_moves_trigon);
    m_total_piece_points = 0;
    BOOST_FOREACH(const Piece& piece, m_pieces)
        m_total_piece_points += piece.get_size();
    if (board_type == board_type_classic || board_type == board_type_duo)
    {
        LIBBOARDGAME_ASSERT(m_nu_pieces == 21);
        LIBBOARDGAME_ASSERT(m_total_piece_points == 89);
    }
    else if (board_type == board_type_trigon)
    {
        LIBBOARDGAME_ASSERT(m_nu_pieces == 22);
        LIBBOARDGAME_ASSERT(m_total_piece_points == 110);
    }
    init_symmetry_info();
}

void BoardConst::create_move(unsigned int piece_index,
                             const Piece::Points& coord_points, Point center)
{
    MovePoints points;
    for (auto i = coord_points.begin(); i != coord_points.end(); ++i)
        points.push_back(Point((*i).x, (*i).y));
    MoveInfo info;
    info.piece = piece_index;
    info.points = points;
    info.center = center;
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
        for (unsigned int i = 0; i < nu_adj_status_index; ++i)
        {
            if (is_compatible_with_adj_status(p, i, points))
                m_moves[i][piece_index][p].push_back(move);
        }
}

void BoardConst::create_moves(unsigned int piece_index)
{
    const Piece& piece = m_pieces[piece_index];
    if (log_move_creation)
        log() << "Creating moves for piece " << piece.get_name() << "\n";
    for (unsigned int i = 0; i < nu_adj_status_index; ++i)
        m_moves[i][piece_index].init(m_geometry);
    Piece::Points points;
    for (GeometryIterator i(m_geometry); i; ++i)
    {
        if (log_move_creation)
            log() << "Creating moves at " << *i << "\n";
        unsigned int x = (*i).get_x();
        unsigned int y = (*i).get_y();
        BOOST_FOREACH(const Transform* transform, piece.get_transforms())
        {
            if (log_move_creation)
                log() << "Transformation " << typeid(*transform).name() << "\n";
            // Pieces are defined such that (0,0) has point type 0. Check if the
            // transformed type is compatible with the location on the board.
            unsigned int point_type = m_geometry.get_point_type(x, y);
            LIBBOARDGAME_ASSERT(transform->get_point_type() == 0);
            if (transform->get_new_point_type() != point_type)
                continue;
            points = piece.get_points();
            transform->transform(points.begin(), points.end());
            sort(points.begin(), points.end());
            auto center_pos =
                find(points.begin(), points.end(), CoordPoint(0, 0));
            LIBBOARDGAME_ASSERT(center_pos != points.end());
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
                create_move(piece_index, points,
                            Point(center_pos->x, center_pos->y));
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
    if (! m_geometry.is_onboard(p))
        return false;
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

void BoardConst::init_adj_status()
{
    m_adj_status.init(m_geometry);
    array<bool, adj_status_nu_adj> forbidden;
    for (GeometryIterator i(m_geometry); i; ++i)
        init_adj_status(*i, forbidden, 0);
}

void BoardConst::init_adj_status(Point p,
                                 array<bool, adj_status_nu_adj>& forbidden,
                                 unsigned int i)
{
    if (i == adj_status_nu_adj || i == m_geometry.get_adj_diag(p).size())
    {
        unsigned int index = 0;
        for (unsigned int j = 0; j < i; ++j)
            if (forbidden[j])
                index |= (1 << j);
        unsigned int n = 0;
        for (AdjDiagIterator j(m_geometry, p); n < i; ++j, ++n)
            if (forbidden[n])
                m_adj_status[p][index].push_back(*j);
        return;
    }
    forbidden[i] = false;
    init_adj_status(p, forbidden, i + 1);
    forbidden[i] = true;
    init_adj_status(p, forbidden, i + 1);
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
    BOOST_FOREACH(Point p_adj, m_adj_status[p][adj_status_index])
        if (points.contains(p_adj))
            return false;
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
