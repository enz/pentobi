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
#include "libboardgame_base/Transform.h"
#include "libboardgame_util/Log.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_base::Transform;
using libboardgame_util::log;

//-----------------------------------------------------------------------------

namespace {

const bool log_move_creation = false;

vector<Piece> create_pieces()
{
    // Define the 21 standard pieces. The piece names are the standard names as
    // http://c2strategy.wordpress.com/2011/04/10/piece-names/. The default
    // orientation is chosen such that it resembles the letter in the piece name
    vector<Piece> pieces;
    pieces.reserve(BoardConst::nu_pieces);
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
    LIBBOARDGAME_ASSERT(pieces.size() == BoardConst::nu_pieces);
    return pieces;
}

}

//-----------------------------------------------------------------------------

BoardConst::BoardConst(unsigned int sz)
    : m_sz(sz),
      m_geometry(*Geometry::get(sz))
{
    m_pieces = create_pieces();
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
    LIBBOARDGAME_ASSERT(sz != 20 || m_move_info.size() == Move::range - 2);
#if LIBBOARDGAME_DEBUG
    unsigned int sum_points = 0;
    BOOST_FOREACH(const Piece& piece, m_pieces)
        sum_points += piece.get_size();
    LIBBOARDGAME_ASSERT(sum_points == total_piece_points);
#endif
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
    for (unsigned int x = 0; x < m_sz; ++x)
        for (unsigned int y = 0; y < m_sz; ++y)
            BOOST_FOREACH(Transform transform, m_pieces[piece].get_transforms())
            {
                points = m_pieces[piece].get_points();
                transform.transform(points.begin(), points.end());
                sort(points.begin(), points.end());
                auto center =
                    find(points.begin(), points.end(), CoordPoint(0, 0));
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
                    if (! p.is_onboard(m_sz))
                    {
                        is_onboard = false;
                        break;
                    }
                }
                if (is_onboard)
                    create_move(piece, points, *center);
            };
}

const BoardConst& BoardConst::get(unsigned int size)
{
    static array<unique_ptr<BoardConst>, Point::max_size + 1> the_board_const;
    if (the_board_const[size].get() == 0)
        the_board_const[size].reset(new BoardConst(size));
    return *the_board_const[size];
}

bool BoardConst::get_piece_index_by_name(const string& name,
                                         unsigned int& index) const
{
    for (unsigned int i = 0; i < nu_pieces; ++i)
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
            if ((*j).is_onboard(m_sz) && ! m_marker[*j])
            {
                m_marker.set(*j);
                info.adj_points.push_back(*j);
            }
    info.attach_points.clear();
    for (auto i = begin; i != end; ++i)
        for (DiagIterator j(m_geometry, *i); j; ++j)
            if ((*j).is_onboard(m_sz) && ! m_marker[*j])
            {
                m_marker.set(*j);
                info.attach_points.push_back(*j);
            }
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
