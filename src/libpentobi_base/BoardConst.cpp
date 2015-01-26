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
#include <functional>
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

// Sort points using the ordering used in blksgf files (switches the direction
// of the y axis!)
inline void sort_piece_points(PiecePoints& points)
{
    auto check = [&](unsigned short a, unsigned short b)
    {
        if ((points[a].y == points[b].y && points[a].x > points[b].x)
                || points[a].y < points[b].y)
            swap(points[a], points[b]);
    };
    // Minimal number of necessary comparisons with sorting networks
    auto size = points.size();
    if (size == 6)
    {
        check(1, 2);
        check(4, 5);
        check(0, 2);
        check(3, 5);
        check(0, 1);
        check(3, 4);
        check(2, 5);
        check(0, 3);
        check(1, 4);
        check(2, 4);
        check(1, 3);
        check(2, 3);
    }
    else if (size == 5)
    {
        check(0, 1);
        check(3, 4);
        check(2, 4);
        check(2, 3);
        check(1, 4);
        check(0, 3);
        check(0, 2);
        check(1, 3);
        check(1, 2);
    }
    else if (size == 4)
    {
        check(0, 1);
        check(2, 3);
        check(0, 2);
        check(1, 3);
        check(1, 2);
    }
    else if (size == 3)
    {
        check(1, 2);
        check(0, 2);
        check(0, 1);
    }
    else if (size == 2)
    {
        check(0, 1);
    }
    else
    {
        LIBBOARDGAME_ASSERT(size == 1);
    }
}

vector<PieceInfo> create_pieces_classic(const Geometry& geo,
                                        const PieceTransforms& transforms)
{
    vector<PieceInfo> pieces;
    // Define the 21 standard pieces. The piece names are the standard names as
    // in http://blokusstrategy.com/?p=48. The default orientation is chosen
    // such that it resembles the letter in the piece name
    pieces.reserve(21);
    pieces.emplace_back("L5",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(1, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("V5",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2), CoordPoint(1, 0),
                                     CoordPoint(2, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("Z5",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("N",
                        PiecePoints{ CoordPoint(-1, 1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2)},
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("W",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(-1, -1),
                                     CoordPoint(0, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("X",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, -1),
                                     CoordPoint(0, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("F",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(1, -1),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("T5",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, -1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("I5",
                        PiecePoints{ CoordPoint(0, 2), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("Y",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(0, 1),
                                     CoordPoint(0, 2) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("P",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 0),
                                     CoordPoint(1, -1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("U",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(-1, -1),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(1, -1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("L4",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("T4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("Z4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("I4",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(0, 2) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("O",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0), CoordPoint(1, -1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("V3",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("I3",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("2",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("1",
                        PiecePoints{ CoordPoint(0, 0) },
                        geo, transforms, CoordPoint(0, 0));
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
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("P",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 0),
                                     CoordPoint(1, -1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("I5",
                        PiecePoints{ CoordPoint(0, 2), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("O",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0), CoordPoint(1, -1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("T4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("Z4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("L4",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("I4",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(0, -2) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("V3",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("I3",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("2",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("1",
                        PiecePoints{ CoordPoint(0, 0) },
                        geo, transforms, CoordPoint(0, 0));
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
    pieces.reserve(22);
    pieces.emplace_back("I6",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(2, -1),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(-1, 1), CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("L6",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(2, -1),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(1, 0));
    pieces.emplace_back("V",
                        PiecePoints{ CoordPoint(-2, -1), CoordPoint(-1, -1),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("S",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(-1, 1), CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("P6",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(-1, 1), CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(1, 0));
    pieces.emplace_back("F",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1),
                                     CoordPoint(2, 1), CoordPoint(1, 2) },
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("W",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(2, 0), CoordPoint(3, 0) },
                        geo, transforms, CoordPoint(1, 0));
    pieces.emplace_back("A6",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(0, 1), CoordPoint(2, 1) },
                        geo, transforms, CoordPoint(1, 0));
    pieces.emplace_back("G",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 1), CoordPoint(2, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("Y",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(-1, 1), CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("X",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(-1, 1),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("O",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(1, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("I5",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(-1, 1),
                                     CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("L5",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("C5",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1),
                                     CoordPoint(2, 1) },
                        geo, transforms, CoordPoint(0, 1));
    pieces.emplace_back("P5",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(1, 0));
    pieces.emplace_back("I4",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(-1, 1), CoordPoint(0, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("C4",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("A4",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0) },
                        geo, transforms, CoordPoint(1, 0));
    pieces.emplace_back("I3",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(1, 0));
    pieces.emplace_back("2",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0) },
                        geo, transforms, CoordPoint(0, 0));
    pieces.emplace_back("1",
                        PiecePoints{ CoordPoint(0, 0) },
                        geo, transforms, CoordPoint(0, 0));
    return pieces;
}

} // namespace

//-----------------------------------------------------------------------------

Marker BoardConst::s_marker;

BoardConst::BoardConst(BoardType board_type, PieceSet piece_set)
    : m_geo(libpentobi_base::get_geometry(board_type)),
      m_nu_attach_points(0)
{
    m_board_type = board_type;
    if (board_type == BoardType::trigon && piece_set == PieceSet::trigon)
    {
        m_transforms.reset(new PieceTransformsTrigon);
        m_pieces = create_pieces_trigon(m_geo, *m_transforms);
        reserve_info(Move::onboard_moves_trigon);
    }
    else if (board_type == BoardType::trigon_3
             && piece_set == PieceSet::trigon)
    {
        m_transforms.reset(new PieceTransformsTrigon);
        m_pieces = create_pieces_trigon(m_geo, *m_transforms);
        reserve_info(Move::onboard_moves_trigon_3);
    }
    else if (board_type == BoardType::classic
             && piece_set == PieceSet::classic)
    {
        m_transforms.reset(new PieceTransformsClassic);
        m_pieces = create_pieces_classic(m_geo, *m_transforms);
        reserve_info(Move::onboard_moves_classic);
    }
    else if (board_type == BoardType::duo && piece_set == PieceSet::junior)
    {
        m_transforms.reset(new PieceTransformsClassic);
        m_pieces = create_pieces_junior(m_geo, *m_transforms);
        reserve_info(Move::onboard_moves_junior);
    }
    else if (board_type == BoardType::duo && piece_set == PieceSet::classic)
    {
        m_transforms.reset(new PieceTransformsClassic);
        m_pieces = create_pieces_classic(m_geo, *m_transforms);
        reserve_info(Move::onboard_moves_duo);
    }
    else
        LIBBOARDGAME_ASSERT(false);
    m_nu_pieces = static_cast<Piece::IntType>(m_pieces.size());
    init_adj_status();
    auto width = m_geo.get_width();
    auto height = m_geo.get_height();
    for (GeometryIterator i(m_geo); i; ++i)
        m_compare_val[*i] =
                (height - m_geo.get_y(*i) - 1) * width + m_geo.get_x(*i);
    create_moves();
    auto nu_moves = m_move_info.size();
    LIBBOARDGAME_UNUSED_IF_NOT_DEBUG(nu_moves);
    if (board_type == BoardType::classic && piece_set == PieceSet::classic)
        LIBBOARDGAME_ASSERT(nu_moves == Move::onboard_moves_classic + 1);
    else if (board_type == BoardType::trigon && piece_set == PieceSet::trigon)
        LIBBOARDGAME_ASSERT(nu_moves == Move::onboard_moves_trigon + 1);
    else if (board_type == BoardType::trigon_3
             && piece_set == PieceSet::trigon)
        LIBBOARDGAME_ASSERT(nu_moves == Move::onboard_moves_trigon_3 + 1);
    else if (board_type == BoardType::duo && piece_set == PieceSet::classic)
        LIBBOARDGAME_ASSERT(nu_moves == Move::onboard_moves_duo + 1);
    else if (board_type == BoardType::duo && piece_set == PieceSet::junior)
        LIBBOARDGAME_ASSERT(nu_moves == Move::onboard_moves_junior + 1);
    else
        LIBBOARDGAME_ASSERT(false);
    m_total_piece_points = 0;
    m_max_piece_size = 0;
    for (const PieceInfo& piece : m_pieces)
    {
        m_max_piece_size = max(m_max_piece_size, piece.get_size());
        m_total_piece_points += piece.get_size();
    }
    if (piece_set == PieceSet::classic)
    {
        LIBBOARDGAME_ASSERT(m_nu_pieces == 21);
        LIBBOARDGAME_ASSERT(m_total_piece_points == 89);
    }
    else if (piece_set == PieceSet::junior)
    {
        LIBBOARDGAME_ASSERT(m_nu_pieces == 12);
        LIBBOARDGAME_ASSERT(m_total_piece_points == 44);
    }
    else if (piece_set == PieceSet::trigon)
    {
        LIBBOARDGAME_ASSERT(m_nu_pieces == 22);
        LIBBOARDGAME_ASSERT(m_total_piece_points == 110);
    }
    if (board_type == BoardType::duo || board_type == BoardType::trigon)
        init_symmetry_info();
}

void BoardConst::create_move(Piece piece, const PiecePoints& coord_points,
                             Point label_pos)
{
    MovePoints points;
    for (auto i = coord_points.begin(); i != coord_points.end(); ++i)
        points.push_back(m_geo.get_point((*i).x, (*i).y));
    MoveInfo info(piece, points);
    MoveInfoExt info_ext;
    set_adj_and_attach_points(info, info_ext);
    MoveInfoExt2 info_ext_2;
    info_ext_2.label_pos = label_pos;
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
    // Unused move infos for Move::null()
    m_move_info.push_back(MoveInfo());
    m_move_info_ext.push_back(MoveInfoExt());
    m_move_info_ext_2.push_back(MoveInfoExt2());

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
                auto size = m_precomp_moves.get_size() - begin;
                m_precomp_moves.set_list_range(*i, j, piece, begin, size);
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
    auto& transforms = piece_info.get_transforms();
    auto nu_transforms = transforms.size();
    vector<PiecePoints> transformed_points(nu_transforms);
    vector<CoordPoint> transformed_label_pos(nu_transforms);
    for (size_t i = 0; i < nu_transforms; ++i)
    {
        auto transform = transforms[i];
        transformed_points[i] = piece_info.get_points();
        transform->transform(transformed_points[i].begin(),
                             transformed_points[i].end());
        sort_piece_points(transformed_points[i]);
        transformed_label_pos[i] =
                transform->get_transformed(piece_info.get_label_pos());
    }
    PiecePoints points;
    // Make outer loop iterator over geometry for better memory locality
    for (GeometryIterator i(m_geo); i; ++i)
    {
        if (log_move_creation)
            log("Creating moves at ", m_geo.to_string(*i));
        auto x = m_geo.get_x(*i);
        auto y = m_geo.get_y(*i);
        for (size_t j = 0; j < nu_transforms; ++j)
        {
            if (log_move_creation)
                log("Transformation ", typeid(*transforms[j]).name());
            auto point_type = m_geo.get_point_type(x, y);
            if (transforms[j]->get_new_point_type() != point_type)
                continue;
            points = transformed_points[j];
            bool is_onboard = true;
            for (auto& p : points)
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
            CoordPoint label_pos = transformed_label_pos[j];
            label_pos.x += x;
            label_pos.y += y;
            create_move(piece, points,
                        m_geo.get_point(label_pos.x, label_pos.y));
        }
    }
}

Move BoardConst::from_string(const string& s) const
{
    string trimmed = to_lower(trim(s));
    if (trimmed == "pass" || trimmed == "null")
        return Move::null();
    vector<string> v = split(trimmed, ',');
    if (v.size() > PieceInfo::max_size)
        throw Exception("illegal move (too many points)");
    MovePoints points;
    for (const auto& s : v)
    {
        Point p;
        if (! m_geo.from_string(s, p))
            throw Exception("illegal move (invalid point)");
        points.push_back(p);
    }
    Move mv;
    if (! find_move(points, mv))
        throw Exception("illegal move");
    return mv;
}

const BoardConst& BoardConst::get(Variant variant)
{
    static map<BoardType, map<PieceSet, unique_ptr<BoardConst>>> board_const;
    auto board_type = libpentobi_base::get_board_type(variant);
    auto piece_set = get_piece_set(variant);
    auto& bc = board_const[board_type][piece_set];
    if (! bc)
        bc.reset(new BoardConst(board_type, piece_set));
    return *bc;
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
    sort(sorted_points);
    Point p = points[0];
    for (Piece::IntType i = 0; i < m_pieces.size(); ++i)
    {
        Piece piece(i);
        if (get_piece_info(piece).get_size() == points.size())
        {
            auto moves = get_moves(piece, p);
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
    for (GeometryIterator i(m_geo); i; ++i)
    {
        auto& adj_status_list = m_adj_status_list[*i];
        for (Point p : m_geo.get_adj(*i))
        {
            if (adj_status_list.size() == PrecompMoves::adj_status_nu_adj)
                break;
            adj_status_list.push_back(p);
        }
        for (Point p : m_geo.get_diag(*i))
        {
            if (adj_status_list.size() == PrecompMoves::adj_status_nu_adj)
                break;
            adj_status_list.push_back(p);
        }
    }
    array<bool, PrecompMoves::adj_status_nu_adj> forbidden;
    for (GeometryIterator i(m_geo); i; ++i)
        init_adj_status(*i, forbidden, 0);
}

void BoardConst::init_adj_status(
                       Point p,
                       array<bool, PrecompMoves::adj_status_nu_adj>& forbidden,
                       unsigned i)
{
    auto& adj_status_list = m_adj_status_list[p];
    if (i == adj_status_list.size())
    {
        unsigned index = 0;
        for (unsigned j = 0; j < i; ++j)
            if (forbidden[j])
                index |= (1 << j);
        unsigned n = 0;
        for (Point j : adj_status_list)
        {
            if (n >= i)
                return;
            if (forbidden[n])
                m_adj_status[p][index].push_back(j);
            ++n;
        }
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
    for (unsigned i = 1; i < m_move_info.size(); ++i)
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
    s_marker.clear();
    for (auto i = begin; i != end; ++i)
        s_marker.set(*i);
    ArrayList<Point, PieceInfo::max_adj> adj_points;
    for (auto i = begin; i != end; ++i)
        m_geo.for_each_adj(*i, [&](Point j) {
            if (! s_marker[j])
            {
                s_marker.set(j);
                adj_points.push_back(j);
            }
        });
    ArrayList<Point, PieceInfo::max_attach> attach_points;
    for (auto i = begin; i != end; ++i)
        m_geo.for_each_diag(*i, [&](Point j) {
            if (! s_marker[j])
            {
                s_marker.set(j);
                attach_points.push_back(j);
            }
        });
    info_ext.init(adj_points, attach_points);
}

inline void BoardConst::sort(MovePoints& points) const
{
    auto check = [&](unsigned short a, unsigned short b)
    {
        if (m_compare_val[points[a]] > m_compare_val[points[b]])
            swap(points[a], points[b]);
    };
    // Minimal number of necessary comparisons with sorting networks
    auto size = points.size();
    if (size == 6)
    {
        check(1, 2);
        check(4, 5);
        check(0, 2);
        check(3, 5);
        check(0, 1);
        check(3, 4);
        check(2, 5);
        check(0, 3);
        check(1, 4);
        check(2, 4);
        check(1, 3);
        check(2, 3);
    }
    else if (size == 5)
    {
        check(0, 1);
        check(3, 4);
        check(2, 4);
        check(2, 3);
        check(1, 4);
        check(0, 3);
        check(0, 2);
        check(1, 3);
        check(1, 2);
    }
    else if (size == 4)
    {
        check(0, 1);
        check(2, 3);
        check(0, 2);
        check(1, 3);
        check(1, 2);
    }
    else if (size == 3)
    {
        check(1, 2);
        check(0, 2);
        check(0, 1);
    }
    else if (size == 2)
    {
        check(0, 1);
    }
    else
        LIBBOARDGAME_ASSERT(size == 1);
}

string BoardConst::to_string(Move mv, bool with_piece_name) const
{
    if (mv.is_null())
        return "pass";
    auto& info = get_move_info(mv);
    ostringstream s;
    if (with_piece_name)
        s << '[' << get_piece_info(info.get_piece()).get_name() << "]";
    bool is_first = true;
    for (Point p : info)
    {
        if (! is_first)
            s << ',';
        else
            is_first = false;
        s << m_geo.to_string(p);
    }
    return s.str();
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
