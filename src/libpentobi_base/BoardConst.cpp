//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardConst.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardConst.h"

#include <algorithm>
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
#include "libboardgame_util/StringUtil.h"

namespace libpentobi_base {

using namespace std;
using libboardgame_base::PointTransfRot180;
using libboardgame_base::RectGeometry;
using libboardgame_base::Transform;
using libboardgame_base::TrigonGeometry;
using libboardgame_util::get_log;
using libboardgame_util::log;
using libboardgame_util::split;
using libboardgame_util::to_lower;
using libboardgame_util::trim;

//-----------------------------------------------------------------------------

namespace {

const bool log_move_creation = false;

vector<PieceInfo> create_pieces_classic(const Geometry& geo,
                                        const PieceTransforms& transforms)
{
    vector<PieceInfo> pieces;
    // Define the 21 standard pieces. The piece names are the standard names as
    // in http://blokusstrategy.com/?p=48. The default orientation is chosen
    // such that it resembles the letter in the piece name
    pieces.reserve(21);
    pieces.emplace_back("X",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, -1),
                                     CoordPoint(0, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 0) },
                        geo, transforms);
    pieces.emplace_back("F",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(1, -1),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms);
    pieces.emplace_back("L5",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(1, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2) },
                        geo, transforms);
    pieces.emplace_back("N",
                        PiecePoints{ CoordPoint(-1, 1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2)},
                        geo, transforms);
    pieces.emplace_back("P",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 0),
                                     CoordPoint(1, -1) },
                        geo, transforms);
    pieces.emplace_back("T5",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, -1) },
                        geo, transforms);
    pieces.emplace_back("U",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(-1, -1),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(1, -1) },
                        geo, transforms);
    pieces.emplace_back("V5",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2), CoordPoint(1, 0),
                                     CoordPoint(2, 0) },
                        geo, transforms);
    pieces.emplace_back("W",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(-1, -1),
                                     CoordPoint(0, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 1) },
                        geo, transforms);
    pieces.emplace_back("Y",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(0, 1),
                                     CoordPoint(0, 2) },
                        geo, transforms);
    pieces.emplace_back("Z5",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 1) },
                        geo, transforms);
    pieces.emplace_back("I5",
                        PiecePoints{ CoordPoint(0, 2), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2) },
                        geo, transforms);
    pieces.emplace_back("O",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0), CoordPoint(1, -1) },
                        geo, transforms);
    pieces.emplace_back("T4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1) },
                        geo, transforms);
    pieces.emplace_back("Z4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms);
    pieces.emplace_back("L4",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 1) },
                        geo, transforms);
    pieces.emplace_back("I4",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(0, 2) },
                        geo, transforms);
    pieces.emplace_back("V3",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0) },
                        geo, transforms);
    pieces.emplace_back("I3",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms);
    pieces.emplace_back("2",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0) },
                        geo, transforms);
    pieces.emplace_back("1",
                        PiecePoints{ CoordPoint(0, 0) },
                        geo, transforms);
    return pieces;
}

vector<PieceInfo> create_pieces_junior(const Geometry& geo,
                                       const PieceTransforms& transforms)
{
    vector<PieceInfo> pieces;
    pieces.reserve(12);
    pieces.emplace_back("L5",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(1, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2) },
                        geo, transforms);
    pieces.emplace_back("P",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 0),
                                     CoordPoint(1, -1) },
                        geo, transforms);
    pieces.emplace_back("I5",
                        PiecePoints{ CoordPoint(0, 2), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2) },
                        geo, transforms);
    pieces.emplace_back("O",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0), CoordPoint(1, -1) },
                        geo, transforms);
    pieces.emplace_back("T4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1) },
                        geo, transforms);
    pieces.emplace_back("Z4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms);
    pieces.emplace_back("L4",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 1) },
                        geo, transforms);
    pieces.emplace_back("I4",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(0, -2) },
                        geo, transforms);
    pieces.emplace_back("V3",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0) },
                        geo, transforms);
    pieces.emplace_back("I3",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms);
    pieces.emplace_back("2",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0) },
                        geo, transforms);
    pieces.emplace_back("1",
                        PiecePoints{ CoordPoint(0, 0) },
                        geo, transforms);
    return pieces;
}

vector<PieceInfo> create_pieces_trigon(const Geometry& geo,
                                       const PieceTransforms& transforms)
{
    vector<PieceInfo> pieces;
    // Define the 22 standard Trigon pieces. The piece names are similar to one
    // of the possible notations from the thread "Trigon book: how to play, how
    // to win" from August 2010 in the Blokus forums
    // http://forum.blokus.refreshed.be/viewtopic.php?f=2&t=2539#p9867
    // apart from that the smallest pieces are named '2' and '1' like in
    // Classic to avoid to many pieces with letter 'I' and that numbers are
    // only used if there is more than one piece with the same letter.
    // For historical reasons, the Trigon transformations apply to point type
    // 1 (upward triangle), so the (0,0) point of a piece must have point
    // type 1, and the pieces are defined such that they resemble the letter of
    // their name when viewed flipped upside-down.
    pieces.reserve(22);
    pieces.emplace_back("I6",
                        PiecePoints{ CoordPoint(-2, -1), CoordPoint(-1, -1),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms);

    pieces.emplace_back("L6",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms);

    pieces.emplace_back("W",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(-2, 0),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0) },
                        geo, transforms);
    pieces.emplace_back("P6",
                        PiecePoints{ CoordPoint(-2, -1), CoordPoint(-1, -1),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1) },
                        geo, transforms);
    pieces.emplace_back("S",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(1, -1),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms);
    pieces.emplace_back("F",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 1), CoordPoint(2, 1) },
                        geo, transforms);
    pieces.emplace_back("V",
                        PiecePoints{ CoordPoint(-1, 1), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(2, 0), CoordPoint(3, 0) },
                        geo, transforms);
    pieces.emplace_back("Y",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(-1, 1), CoordPoint(0, 1),
                                     CoordPoint(1, 1), CoordPoint(-1, 2) },
                        geo, transforms);
    pieces.emplace_back("A6",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(-1, -1),
                                     CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(1, -1) },
                        geo, transforms);
    pieces.emplace_back("G",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(1, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(0, 1) },
                        geo, transforms);
    pieces.emplace_back("O",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(-1, -1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0), CoordPoint(1, -1) },
                        geo, transforms);
    pieces.emplace_back("X",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(-1, 1),
                                     CoordPoint(0, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 0), CoordPoint(1, 1) },
                        geo, transforms);
    pieces.emplace_back("I5",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1),
                                     CoordPoint(1, 2) },
                        geo, transforms);
    pieces.emplace_back("P5",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms);
    pieces.emplace_back("L5",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms);
    pieces.emplace_back("C5",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0) },
                        geo, transforms);
    pieces.emplace_back("I4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms);
    pieces.emplace_back("C4",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms);
    pieces.emplace_back("A4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 0) },
                        geo, transforms);
    pieces.emplace_back("I3",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms);
    pieces.emplace_back("2",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0) },
                        geo, transforms);
    pieces.emplace_back("1",
                        PiecePoints{ CoordPoint(0, 0) },
                        geo, transforms);
    return pieces;
}

const Geometry& create_geometry(BoardType board_type)
{
    if (board_type == BoardType::classic)
        return RectGeometry<Point>::get(20, 20);
    else if (board_type == BoardType::duo)
        return RectGeometry<Point>::get(14, 14);
    else if (board_type == BoardType::trigon)
        return TrigonGeometry<Point>::get(9);
    else
    {
        LIBBOARDGAME_ASSERT(board_type == BoardType::trigon_3);
        return TrigonGeometry<Point>::get(8);
    }
}

} // namespace

//-----------------------------------------------------------------------------

BoardConst::BoardConst(BoardType board_type, Variant variant)
    : m_geo(create_geometry(board_type)),
      m_nu_attach_points(0)
{
    m_board_type = board_type;
    if (board_type == BoardType::trigon)
    {
        m_transforms.reset(new PieceTransformsTrigon());
        m_pieces = create_pieces_trigon(m_geo, *m_transforms);
        reserve_info(Move::onboard_moves_trigon);
    }
    else if (board_type == BoardType::trigon_3)
    {
        m_transforms.reset(new PieceTransformsTrigon());
        m_pieces = create_pieces_trigon(m_geo, *m_transforms);
        reserve_info(Move::onboard_moves_trigon_3);
    }
    else if (board_type == BoardType::classic)
    {
        m_transforms.reset(new PieceTransformsClassic());
        m_pieces = create_pieces_classic(m_geo, *m_transforms);
        reserve_info(Move::onboard_moves_classic);
    }
    else if (variant == Variant::junior)
    {
        m_transforms.reset(new PieceTransformsClassic());
        m_pieces = create_pieces_junior(m_geo, *m_transforms);
        reserve_info(Move::onboard_moves_junior);
    }
    else
    {
        LIBBOARDGAME_ASSERT(variant == Variant::duo);
        m_transforms.reset(new PieceTransformsClassic());
        m_pieces = create_pieces_classic(m_geo, *m_transforms);
        reserve_info(Move::onboard_moves_duo);
    }
    m_nu_pieces = static_cast<Piece::IntType>(m_pieces.size());
    init_adj_status();
    create_moves();
    if (board_type == BoardType::classic)
        LIBBOARDGAME_ASSERT(m_move_info.size() == Move::onboard_moves_classic);
    else if (board_type == BoardType::trigon)
        LIBBOARDGAME_ASSERT(m_move_info.size() == Move::onboard_moves_trigon);
    else if (board_type == BoardType::trigon_3)
        LIBBOARDGAME_ASSERT(m_move_info.size() == Move::onboard_moves_trigon_3);
    else if (variant == Variant::duo)
        LIBBOARDGAME_ASSERT(m_move_info.size() == Move::onboard_moves_duo);
    else if (variant == Variant::junior)
        LIBBOARDGAME_ASSERT(m_move_info.size() == Move::onboard_moves_junior);
    m_total_piece_points = 0;
    m_max_piece_size = 0;
    for (const PieceInfo& piece : m_pieces)
    {
        m_max_piece_size = max(m_max_piece_size, piece.get_size());
        m_total_piece_points += piece.get_size();
    }
    if (board_type == BoardType::classic || variant == Variant::duo)
    {
        LIBBOARDGAME_ASSERT(m_nu_pieces == 21);
        LIBBOARDGAME_ASSERT(m_total_piece_points == 89);
    }
    else if (variant == Variant::junior)
    {
        LIBBOARDGAME_ASSERT(m_nu_pieces == 12);
        LIBBOARDGAME_ASSERT(m_total_piece_points == 44);
    }
    else if (board_type == BoardType::trigon
             || board_type == BoardType::trigon_3)
    {
        LIBBOARDGAME_ASSERT(m_nu_pieces == 22);
        LIBBOARDGAME_ASSERT(m_total_piece_points == 110);
    }
    if (variant == Variant::duo || variant == Variant::junior
        || variant == Variant::trigon_2)
        init_symmetry_info();
}

void BoardConst::create_move(Piece piece, const PiecePoints& coord_points,
                             Point center)
{
    MovePoints points;
    auto width = m_geo.get_width();
    for (auto i = coord_points.begin(); i != coord_points.end(); ++i)
        points.push_back(Point((*i).x, (*i).y, width));
    MoveInfo info(piece, points);
    MoveInfoExt info_ext;
    set_adj_and_attach_points(info, info_ext);
    MoveInfoExt2 info_ext_2;
    info_ext_2.center = center;
    info_ext_2.breaks_symmetry = false;
    info_ext_2.symmetric_move = Move::null();
    m_move_info.push_back(info);
    m_move_info_ext.push_back(info_ext);
    m_move_info_ext_2.push_back(info_ext_2);
    m_nu_attach_points[piece] =
        max(m_nu_attach_points[piece],
            static_cast<unsigned>(info_ext.size_attach_points));
    Move mv(static_cast<Move::IntType>(m_move_info.size() - 1));
    if (log_move_creation)
    {
        Grid<char> grid;
        grid.fill('.', m_geo);
        for (Point p : info)
            grid[p] = 'O';
        for (auto i = info_ext.begin_adj(); i != info_ext.end_adj(); ++i)
            grid[*i] = '+';
        for (auto i = info_ext.begin_attach(); i != info_ext.end_attach(); ++i)
            grid[*i] = '*';
        log("Move ", mv.to_int(), ":\n", grid.to_string(m_geo));
    }
    for (Point p : info)
        for (unsigned i = 0; i < PrecompMoves::nu_adj_status; ++i)
            if (is_compatible_with_adj_status(p, i, info))
                (*m_full_move_table)[i][piece][p].push_back(mv);
}

void BoardConst::create_moves()
{
    m_full_move_table.reset(new FullMoveTable);
    for (Piece::IntType i = 0; i < m_nu_pieces; ++i)
        create_moves(Piece(i));
    for (GeometryIterator i(m_geo); i; ++i)
        for (unsigned j = 0; j < PrecompMoves::nu_adj_status; ++j)
            for (Piece::IntType k = 0; k < m_nu_pieces; ++k)
            {
                Piece piece(k);
                auto& list = (*m_full_move_table)[j][piece][*i];
                auto begin = m_precomp_moves.get_size();
                for (unsigned l = 0; l < list.size(); ++l)
                    m_precomp_moves.push_move(list[l]);
                auto end = m_precomp_moves.get_size() - begin;
                m_precomp_moves.set_list_range(*i, j, piece, begin, end);
            }
    if (log_move_creation)
        log("Created moves: ", m_move_info.size(), ", precomputed: ",
            m_precomp_moves.get_size());
    m_full_move_table.reset(nullptr); // Free space, no longer needed
}

void BoardConst::create_moves(Piece piece)
{
    auto& piece_info = m_pieces[piece.to_int()];
    if (log_move_creation)
        log("Creating moves for piece ", piece_info.get_name());
    PiecePoints points;
    auto width = m_geo.get_width();
    auto height = m_geo.get_height();
    for (GeometryIterator i(m_geo); i; ++i)
    {
        if (log_move_creation)
            log("Creating moves at ", WritePoint(*i, width, height));
        auto x = (*i).get_x(width);
        auto y = (*i).get_y(width);
        for (const Transform* transform : piece_info.get_transforms())
        {
            if (log_move_creation)
                log("Transformation ", typeid(*transform).name());
            auto point_type = m_geo.get_point_type(x, y);
            if (transform->get_new_point_type() != point_type)
                continue;
            points = piece_info.get_points();
            transform->transform(points.begin(), points.end());
            sort(points.begin(), points.end());
            bool is_onboard = true;
            for (CoordPoint& p : points)
            {
                p.x += x;
                p.y += y;
                if (! m_geo.is_onboard(p))
                {
                    is_onboard = false;
                    break;
                }
            }
            if (! is_onboard)
                continue;
            LIBBOARDGAME_ASSERT(points.contains(CoordPoint(x, y)));
            create_move(piece, points, Point(x, y, width));
        }
    }
}

Move BoardConst::from_string(const string& s) const
{
    string trimmed = to_lower(trim(s));
    if (trimmed == "pass")
        return Move::pass();
    else if (trimmed == "null")
        return Move::null();
    vector<string> v = split(trimmed, ',');
    if (v.size() > PieceInfo::max_size)
        throw Exception("illegal move (too many points)");
    MovePoints points;
    auto width = m_geo.get_width();
    auto height = m_geo.get_height();
    for (const string& p : v)
        points.push_back(Point::from_string(p, width, height));
    Move mv;
    if (! find_move(points, mv))
        throw Exception("illegal move");
    return mv;
}

const BoardConst& BoardConst::get(Variant variant)
{
    static unique_ptr<BoardConst> board_const_classic;
    static unique_ptr<BoardConst> board_const_duo;
    static unique_ptr<BoardConst> board_const_junior;
    static unique_ptr<BoardConst> board_const_trigon;
    static unique_ptr<BoardConst> board_const_trigon_2;
    static unique_ptr<BoardConst> board_const_trigon_3;
    if (variant == Variant::classic || variant == Variant::classic_2)
    {
        if (! board_const_classic)
            board_const_classic.reset(new BoardConst(BoardType::classic,
                                                     Variant::classic));
        return *board_const_classic;
    }
    else if (variant == Variant::duo)
    {
        if (! board_const_duo)
            board_const_duo.reset(new BoardConst(BoardType::duo,
                                                 Variant::duo));
        return *board_const_duo;
    }
    else if (variant == Variant::junior)
    {
        if (! board_const_junior)
            board_const_junior.reset(new BoardConst(BoardType::duo,
                                                    Variant::junior));
        return *board_const_junior;
    }
    else if (variant == Variant::trigon)
    {
        if (! board_const_trigon)
            board_const_trigon.reset(new BoardConst(BoardType::trigon,
                                                    Variant::trigon));
        return *board_const_trigon;
    }
    else if (variant == Variant::trigon_2)
    {
        if (! board_const_trigon_2)
            board_const_trigon_2.reset(new BoardConst(BoardType::trigon,
                                                      Variant::trigon_2));
        return *board_const_trigon_2;
    }
    else
    {
        LIBBOARDGAME_ASSERT(variant == Variant::trigon_3);
        if (! board_const_trigon_3)
            board_const_trigon_3.reset(new BoardConst(BoardType::trigon_3,
                                                      Variant::trigon_3));
        return *board_const_trigon_3;
    }
}

bool BoardConst::get_piece_by_name(const string& name, Piece& piece) const
{
    for (Piece::IntType i = 0; i < m_nu_pieces; ++i)
        if (get_piece_info(Piece(i)).get_name() == name)
        {
            piece = Piece(i);
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
    if (! m_geo.is_onboard(p))
        return false;
    for (Piece::IntType i = 0; i < m_pieces.size(); ++i)
    {
        Piece piece(i);
        if (get_piece_info(piece).get_size() == points.size())
        {
            Board::LocalMovesListRange moves = get_moves(piece, p);
            for (auto j = moves.begin(); j != moves.end(); ++j)
                if (equal(sorted_points.begin(),
                          sorted_points.end(),
                          m_move_info[j->to_int()].begin()))
                {
                    move = *j;
                    return true;
                }
        }
    }
    return false;
}

void BoardConst::init_adj_status()
{
    array<bool, PrecompMoves::adj_status_nu_adj> forbidden;
    for (GeometryIterator i(m_geo); i; ++i)
        init_adj_status(*i, forbidden, 0);
}

void BoardConst::init_adj_status(
                       Point p,
                       array<bool, PrecompMoves::adj_status_nu_adj>& forbidden,
                       unsigned i)
{
    if (i == PrecompMoves::adj_status_nu_adj
        || i == m_geo.get_adj_diag(p).size())
    {
        unsigned index = 0;
        for (unsigned j = 0; j < i; ++j)
            if (forbidden[j])
                index |= (1 << j);
        unsigned n = 0;
        for (AdjDiagIterator j(m_geo, p); n < i; ++j, ++n)
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
    PointTransfRot180<Point> transform;
    symmetric_points.init(m_geo, transform);
    for (unsigned i = 0; i < m_move_info.size(); ++i)
    {
        const auto& info = m_move_info[i];
        auto& info_ext_2 = m_move_info_ext_2[i];
        MovePoints sym_points;
        info_ext_2.breaks_symmetry = false;
        for (Point p : info)
        {
            if (info.contains(symmetric_points[p]))
                info_ext_2.breaks_symmetry = true;
            sym_points.push_back(symmetric_points[p]);
        }
        find_move(sym_points, info_ext_2.symmetric_move);
    }
}

bool BoardConst::is_compatible_with_adj_status(Point p, unsigned adj_status,
                                               const MoveInfo& info) const
{
    for (Point p_adj : m_adj_status[p][adj_status])
        if (info.contains(p_adj))
            return false;
    return true;
}

void BoardConst::reserve_info(size_t nu_moves)
{
    m_move_info.reserve(nu_moves);
    m_move_info_ext.reserve(nu_moves);
    m_move_info_ext_2.reserve(nu_moves);
}

void BoardConst::set_adj_and_attach_points(const MoveInfo& info,
                                           MoveInfoExt& info_ext)
{
    auto begin = info.begin();
    auto end = info.end();
    m_marker.clear();
    for (auto i = begin; i != end; ++i)
        m_marker.set(*i);
    ArrayList<Point, PieceInfo::max_adj> adj_points;
    for (auto i = begin; i != end; ++i)
        for (AdjIterator j(m_geo, *i); j; ++j)
            if (m_geo.is_onboard(*j) && ! m_marker[*j])
            {
                m_marker.set(*j);
                adj_points.push_back(*j);
            }
    ArrayList<Point, PieceInfo::max_attach> attach_points;
    for (auto i = begin; i != end; ++i)
        for (DiagIterator j(m_geo, *i); j; ++j)
            if (m_geo.is_onboard(*j) && ! m_marker[*j])
            {
                m_marker.set(*j);
                attach_points.push_back(*j);
            }
    info_ext.init(adj_points, attach_points);
}

string BoardConst::to_string(Move mv, bool with_piece_name) const
{
    if (mv.is_null())
        return "null";
    if (mv.is_pass())
        return "pass";
    auto& info = get_move_info(mv);
    ostringstream s;
    if (with_piece_name)
        s << '[' << get_piece_info(info.get_piece()).get_name() << "]";
    bool is_first = true;
    auto width = m_geo.get_width();
    auto height = m_geo.get_height();
    for (Point p : info)
    {
        if (! is_first)
            s << ',';
        else
            is_first = false;
        p.write(s, width, height);
    }
    return s.str();
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
