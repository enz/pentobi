//-----------------------------------------------------------------------------
/** @file BoardConst.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardConst.h"

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
{
    m_sz = sz;
    m_pieces = create_pieces();
    init_dist_to_center();
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
                             CoordPoint center, unsigned int x, unsigned int y)
{
    MovePoints points;
    BOOST_FOREACH(const CoordPoint& p, coord_points)
        points.push_back(Point(x + p.x, y + p.y));
    MoveInfo info;
    info.piece = piece;
    info.points = points;
    info.center = Point(x + center.x, y + center.y);
    set_adj_and_corner_points(info);
    info.dist_to_center = numeric_limits<unsigned int>::max();
    BOOST_FOREACH(Point p, info.points)
        info.dist_to_center = min(m_dist_to_center[p], info.dist_to_center);
    m_move_info.push_back(info);
    Move move(static_cast<unsigned int>(m_move_info.size() - 1));
    if (log_move_creation)
    {
        Grid<char> grid(m_sz, '.');
        BOOST_FOREACH(Point p, info.points)
            grid[p] = 'O';
        BOOST_FOREACH(Point p, info.adj_points)
            grid[p] = '+';
        BOOST_FOREACH(Point p, info.corner_points)
            grid[p] = '*';
        log() << "Move " << move.to_int() << ":\n" << grid
              << "dist_to_center=" << info.dist_to_center << '\n';
    }
    BOOST_FOREACH(Point p, points)
    {
        m_moves_at[piece][p].push_back(move);
        for (unsigned int i = 0; i < 4; ++i)
        {
            Direction dir = Direction::get_enum_diag(i);
            if (is_compatible_with_diag(p, dir, points))
                m_moves_at_diag[i][piece][p].push_back(move);
        }
    }
}

void BoardConst::create_moves(unsigned int piece)
{
    m_moves_at[piece].init(m_sz);
    for (unsigned int i = 0; i < 4; ++i)
        m_moves_at_diag[i][piece].init(m_sz);
    Piece::Points points;
    BOOST_FOREACH(Transform transform, m_pieces[piece].get_transforms())
    {
        points = m_pieces[piece].get_points();
        transform.transform(points.begin(), points.end());
        sort(points.begin(), points.end());
        auto center_pos = find(points.begin(), points.end(), CoordPoint(0, 0));
        LIBBOARDGAME_ASSERT(center_pos != points.end());
        unsigned int width;
        unsigned int height;
        CoordPoint::normalize_offset(points.begin(), points.end(),
                                     width, height);
        CoordPoint center = *center_pos;
        for (unsigned int x = 0; x <= m_sz - width; ++x)
            for (unsigned int y = 0; y <= m_sz - height; ++y)
                create_move(piece, points, center, x, y);
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
            const vector<Move>& moves = m_moves_at[i][p];
            for (unsigned int j = 0; j < moves.size(); ++j)
                if (m_move_info[moves[j].to_int()].points == sorted_points)
                {
                    move = m_moves_at[i][p][j];
                    return true;
                }
        }
    return false;
}

void BoardConst::init_dist_to_center()
{
    LIBBOARDGAME_ASSERT(m_sz % 2 == 0);
    unsigned int center1 = m_sz / 2 - 1;
    unsigned int center2 = center1 + 1;
    m_dist_to_center.init(m_sz);
    for (unsigned int x = 0; x < m_sz; ++x)
        for (unsigned int y = 0; y < m_sz; ++y)
        {
            unsigned int dist_x = (x <= center1 ? center1 - x : x - center2);
            unsigned int dist_y = (y <= center1 ? center1 - y : y - center2);
            m_dist_to_center[Point(x, y)] =
                (unsigned int)(sqrt(dist_x * dist_x + dist_y * dist_y));
        }
    if (log_move_creation)
        log() << "Dist to center:\n" << m_dist_to_center;
}

void BoardConst::init_symmetry_info()
{
    SymmetricPoints symmetric_points;
    symmetric_points.init(m_sz);
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

bool BoardConst::is_compatible_with_diag(Point p, Direction dir,
                                         const MovePoints& points)
{
    Point diag = p.get_neighbor(dir);
    if (! diag.is_onboard(m_sz))
        return false;
    BOOST_FOREACH(Point p2, points)
        if (p2.is_adj(diag))
            return false;
    return true;
}

void BoardConst::set_adj_and_corner_points(MoveInfo& info)
{
    // TODO: Using LIBBOARDGAME_FOREACH_ADJ macros are intended for speed
    // optimization and shouldn't be used here. Replace by an adjacent point
    // iterator that does not unroll the loops, once such an iterator exists.
    m_marker.clear();
    BOOST_FOREACH(Point p, info.points)
        m_marker.set(p);
    info.adj_points.clear();
    BOOST_FOREACH(Point p, info.points)
        LIBBOARDGAME_FOREACH_ADJ(p, p_adj,
            if (p_adj.is_onboard(m_sz) && ! m_marker[p_adj]
                && ! info.adj_points.contains(p_adj))
                info.adj_points.push_back(p_adj));
    info.corner_points.clear();
    BOOST_FOREACH(Point p, info.points)
        LIBBOARDGAME_FOREACH_DIAG(p, p_diag,
            if (p_diag.is_onboard(m_sz) && ! m_marker[p_diag]
                && ! info.corner_points.contains(p_diag))
            {
                bool is_forbidden = false;
                LIBBOARDGAME_FOREACH_ADJ(p_diag, p_diag_adj,
                    if (m_marker[p_diag_adj])
                        is_forbidden = true; )
                if (! is_forbidden)
                    info.corner_points.push_back(p_diag);
            })
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
