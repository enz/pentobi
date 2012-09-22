//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardConst.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardConst.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
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
using boost::is_any_of;
using boost::split;
using boost::trim_copy;
using boost::algorithm::to_lower_copy;
using libboardgame_base::RectGeometry;
using libboardgame_base::Transform;
using libboardgame_base::TrigonGeometry;
using libboardgame_util::log;

//-----------------------------------------------------------------------------

namespace {

const bool log_move_creation = false;

vector<PieceInfo> create_pieces_classic(const Geometry& geometry,
                                        const PieceTransforms& transforms)
{
    vector<PieceInfo> pieces;
    // Define the 21 standard pieces. The piece names are the standard names as
    // in http://blokusstrategy.com/?p=48. The default orientation is chosen
    // such that it resembles the letter in the piece name
    pieces.reserve(21);
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        pieces.push_back(PieceInfo("X", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, -1));
        pieces.push_back(PieceInfo("F", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        pieces.push_back(PieceInfo("L5", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        pieces.push_back(PieceInfo("N", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(PieceInfo("P", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(PieceInfo("T5", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(PieceInfo("U", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(2, 0));
        pieces.push_back(PieceInfo("V5", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(PieceInfo("W", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, -2));
        pieces.push_back(PieceInfo("Y", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(PieceInfo("Z5", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, -2));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        pieces.push_back(PieceInfo("I5", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(PieceInfo("O", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(0, -1));
        pieces.push_back(PieceInfo("T4", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(PieceInfo("Z4", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(PieceInfo("L4", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        pieces.push_back(PieceInfo("I4", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        pieces.push_back(PieceInfo("V3", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        pieces.push_back(PieceInfo("I3", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        pieces.push_back(PieceInfo("2", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, 0));
        pieces.push_back(PieceInfo("1", points, geometry, transforms));
    }
    return pieces;
}

vector<PieceInfo> create_pieces_junior(const Geometry& geometry,
                                       const PieceTransforms& transforms)
{
    vector<PieceInfo> pieces;
    pieces.reserve(12);
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        pieces.push_back(PieceInfo("L5", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(PieceInfo("P", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, -2));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        pieces.push_back(PieceInfo("I5", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(PieceInfo("O", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(0, -1));
        pieces.push_back(PieceInfo("T4", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(PieceInfo("Z4", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(PieceInfo("L4", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 2));
        pieces.push_back(PieceInfo("I4", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        pieces.push_back(PieceInfo("V3", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        pieces.push_back(PieceInfo("I3", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        pieces.push_back(PieceInfo("2", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, 0));
        pieces.push_back(PieceInfo("1", points, geometry, transforms));
    }
    return pieces;
}

vector<PieceInfo> create_pieces_trigon(const Geometry& geometry,
                                       const PieceTransforms& transforms)
{
    vector<PieceInfo> pieces;
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
        PiecePoints points;
        points.push_back(CoordPoint(-2, -1));
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(PieceInfo("I6", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(PieceInfo("L6", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(-2, 0));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(2, 0));
        pieces.push_back(PieceInfo("W", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-2, -1));
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(0, 1));
        pieces.push_back(PieceInfo("P6", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(PieceInfo("S", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        points.push_back(CoordPoint(2, 1));
        pieces.push_back(PieceInfo("F", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(2, 0));
        points.push_back(CoordPoint(3, 0));
        pieces.push_back(PieceInfo("V", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        points.push_back(CoordPoint(-1, 2));
        pieces.push_back(PieceInfo("Y", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(PieceInfo("A6", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        pieces.push_back(PieceInfo("G", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, -1));
        pieces.push_back(PieceInfo("O", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(-1, 1));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(PieceInfo("X", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        points.push_back(CoordPoint(1, 2));
        pieces.push_back(PieceInfo("I5", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(0, 1));
        pieces.push_back(PieceInfo("P5", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        pieces.push_back(PieceInfo("L5", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, -1));
        points.push_back(CoordPoint(0, -1));
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        pieces.push_back(PieceInfo("C5", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(PieceInfo("I4", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(1, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 1));
        pieces.push_back(PieceInfo("C4", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        points.push_back(CoordPoint(1, 0));
        pieces.push_back(PieceInfo("A4", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        points.push_back(CoordPoint(0, 1));
        pieces.push_back(PieceInfo("I3", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(-1, 0));
        points.push_back(CoordPoint(0, 0));
        pieces.push_back(PieceInfo("2", points, geometry, transforms));
    }
    {
        PiecePoints points;
        points.push_back(CoordPoint(0, 0));
        pieces.push_back(PieceInfo("1", points, geometry, transforms));
    }
    return pieces;
}

const Geometry& create_geometry(BoardType board_type)
{
    if (board_type == board_type_classic)
        return *RectGeometry<Point>::get(20, 20);
    else if (board_type == board_type_duo)
        return *RectGeometry<Point>::get(14, 14);
    else if (board_type == board_type_trigon)
        return *TrigonGeometry<Point>::get(9);
    else
    {
        LIBBOARDGAME_ASSERT(board_type == board_type_trigon_3);
        return *TrigonGeometry<Point>::get(8);
    }
}

} // namespace

//-----------------------------------------------------------------------------

BoardConst::BoardConst(BoardType board_type, Variant variant)
    : m_geometry(create_geometry(board_type))
{
    // TODO: Better distinction between board type and set of pieces.
    // Currently the variant parameter is used only to distinct between
    // the set of pieces in Duo and Junior, otherwise the parameter is ignored
    // because the set of pieces is derived from the board type.
    m_board_type = board_type;
    if (board_type == board_type_trigon)
    {
        m_transforms.reset(new PieceTransformsTrigon());
        m_pieces = create_pieces_trigon(m_geometry, *m_transforms);
        m_move_info.reserve(Move::onboard_moves_trigon);
    }
    else if (board_type == board_type_trigon_3)
    {
        m_transforms.reset(new PieceTransformsTrigon());
        m_pieces = create_pieces_trigon(m_geometry, *m_transforms);
        m_move_info.reserve(Move::onboard_moves_trigon_3);
    }
    else if (board_type == board_type_classic)
    {
        m_transforms.reset(new PieceTransformsClassic());
        m_pieces = create_pieces_classic(m_geometry, *m_transforms);
        m_move_info.reserve(Move::onboard_moves_classic);
    }
    else if (variant == variant_junior)
    {
        m_transforms.reset(new PieceTransformsClassic());
        m_pieces = create_pieces_junior(m_geometry, *m_transforms);
        m_move_info.reserve(Move::onboard_moves_junior);
    }
    else
    {
        LIBBOARDGAME_ASSERT(variant == variant_duo);
        m_transforms.reset(new PieceTransformsClassic());
        m_pieces = create_pieces_classic(m_geometry, *m_transforms);
        m_move_info.reserve(Move::onboard_moves_duo);
    }
    m_nu_pieces = static_cast<unsigned>(m_pieces.size());
    m_max_attach_points.fill(0);
    init_adj_status();
    create_moves();
    if (board_type == board_type_classic)
        LIBBOARDGAME_ASSERT(m_move_info.size() == Move::onboard_moves_classic);
    else if (board_type == board_type_trigon)
        LIBBOARDGAME_ASSERT(m_move_info.size() == Move::onboard_moves_trigon);
    else if (board_type == board_type_trigon_3)
        LIBBOARDGAME_ASSERT(m_move_info.size() == Move::onboard_moves_trigon_3);
    else if (variant == variant_duo)
        LIBBOARDGAME_ASSERT(m_move_info.size() == Move::onboard_moves_duo);
    else if (variant == variant_junior)
        LIBBOARDGAME_ASSERT(m_move_info.size() == Move::onboard_moves_junior);
    m_total_piece_points = 0;
    BOOST_FOREACH(const PieceInfo& piece, m_pieces)
        m_total_piece_points += piece.get_size();
    if (board_type == board_type_classic || variant == variant_duo)
    {
        LIBBOARDGAME_ASSERT(m_nu_pieces == 21);
        LIBBOARDGAME_ASSERT(m_total_piece_points == 89);
    }
    else if (variant == variant_junior)
    {
        LIBBOARDGAME_ASSERT(m_nu_pieces == 12);
        LIBBOARDGAME_ASSERT(m_total_piece_points == 44);
    }
    else if (board_type == board_type_trigon
             || board_type == board_type_trigon_3)
    {
        LIBBOARDGAME_ASSERT(m_nu_pieces == 22);
        LIBBOARDGAME_ASSERT(m_total_piece_points == 110);
    }
    init_symmetry_info();
}

void BoardConst::create_move(Piece piece, const PiecePoints& coord_points,
                             Point center)
{
    MovePoints points;
    for (auto i = coord_points.begin(); i != coord_points.end(); ++i)
        points.push_back(Point((*i).x, (*i).y));
    MoveInfo info;
    MoveInfoExt info_ext;
    info.piece = piece;
    info.points = points;
    info_ext.center = center;
    set_adj_and_attach_points(info, info_ext);
    m_move_info.push_back(info);
    m_move_info_ext.push_back(info_ext);
    Move move(static_cast<unsigned>(m_move_info.size() - 1));
    if (log_move_creation)
    {
        Grid<char> grid(m_geometry, '.');
        BOOST_FOREACH(Point p, info.points)
            grid[p] = 'O';
        BOOST_FOREACH(Point p, info_ext.adj_points)
            grid[p] = '+';
        BOOST_FOREACH(Point p, info_ext.attach_points)
            grid[p] = '*';
        log() << "Move " << move.to_int() << ":\n" << grid << '\n';
    }
    BOOST_FOREACH(Point p, points)
        for (unsigned i = 0; i < nu_adj_status_index; ++i)
        {
            if (is_compatible_with_adj_status(p, i, points))
            {
                (*m_full_move_table)[i][piece][p].push_back(move);
                ++m_move_lists_sum_length;
            }
        }
}

void BoardConst::create_moves()
{
    m_full_move_table.reset(new FullMoveTable);
    m_moves_range.init(m_geometry);
    m_move_lists_sum_length = 0;
    for (unsigned i = 0; i < m_nu_pieces; ++i)
        create_moves(Piece(i));
    if (log_move_creation)
        log() << "Created moves: " << m_move_info.size() << '\n';
    m_move_lists.reset(new Move[m_move_lists_sum_length]);
    unsigned current = 0;
    for (GeometryIterator i(m_geometry); i; ++i)
        for (unsigned j = 0; j < nu_adj_status_index; ++j)
            for (unsigned k = 0; k < m_nu_pieces; ++k)
            {
                Piece piece(k);
                unsigned begin = current;
                const LocalMovesList& list = (*m_full_move_table)[j][piece][*i];
                for (unsigned l = 0; l < list.size(); ++l)
                    m_move_lists[current++] = list[l];
                unsigned end = current;
                m_moves_range[*i][j][piece] = make_pair(begin, end);
            }
    m_full_move_table.reset(0); // Free space, no longer needed
}

void BoardConst::create_moves(Piece piece)
{
    const PieceInfo& piece_info = m_pieces[piece.to_int()];
    if (log_move_creation)
        log() << "Creating moves for piece " << piece_info.get_name() << "\n";
    for (unsigned i = 0; i < nu_adj_status_index; ++i)
        (*m_full_move_table)[i][piece].init(m_geometry);
    PiecePoints points;
    for (GeometryIterator i(m_geometry); i; ++i)
    {
        if (log_move_creation)
            log() << "Creating moves at " << *i << "\n";
        unsigned x = (*i).get_x();
        unsigned y = (*i).get_y();
        BOOST_FOREACH(const Transform* transform, piece_info.get_transforms())
        {
            if (log_move_creation)
                log() << "Transformation " << typeid(*transform).name() << "\n";
            // Pieces are defined such that (0,0) has point type 0. Check if the
            // transformed type is compatible with the location on the board.
            unsigned point_type = m_geometry.get_point_type(x, y);
            LIBBOARDGAME_ASSERT(transform->get_point_type() == 0);
            if (transform->get_new_point_type() != point_type)
                continue;
            points = piece_info.get_points();
            transform->transform(points.begin(), points.end());
            sort(points.begin(), points.end());
            auto center = find(points.begin(), points.end(), CoordPoint(0, 0));
            LIBBOARDGAME_ASSERT(center != points.end());
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
                create_move(piece, points, Point(center->x, center->y));
        }
    }
}

Move BoardConst::from_string(const string& s) const
{
    string trimmed = to_lower_copy(trim_copy(s));
    if (trimmed == "pass")
        return Move::pass();
    else if (trimmed == "null")
        return Move::null();
    vector<string> v;
    split(v, trimmed, is_any_of(","));
    if (v.size() > PieceInfo::max_size)
        throw Exception("illegal move (too many points)");
    MovePoints points;
    BOOST_FOREACH(const string& p, v)
        points.push_back(Point::from_string(p));
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
    static unique_ptr<BoardConst> board_const_trigon_3;
    if (variant == variant_classic || variant == variant_classic_2)
    {
        if (board_const_classic.get() == 0)
            board_const_classic.reset(new BoardConst(board_type_classic,
                                                     variant_classic));
        return *board_const_classic;
    }
    else if (variant == variant_duo)
    {
        if (board_const_duo.get() == 0)
            board_const_duo.reset(new BoardConst(board_type_duo,
                                                 variant_duo));
        return *board_const_duo;
    }
    else if (variant == variant_junior)
    {
        if (board_const_junior.get() == 0)
            board_const_junior.reset(new BoardConst(board_type_duo,
                                                    variant_junior));
        return *board_const_junior;
    }
    else if (variant == variant_trigon || variant == variant_trigon_2)
    {
        if (board_const_trigon.get() == 0)
            board_const_trigon.reset(new BoardConst(board_type_trigon,
                                                    variant_trigon));
        return *board_const_trigon;
    }
    else
    {
        LIBBOARDGAME_ASSERT(variant == variant_trigon_3);
        if (board_const_trigon_3.get() == 0)
            board_const_trigon_3.reset(new BoardConst(board_type_trigon_3,
                                                      variant_trigon_3));
        return *board_const_trigon_3;
    }
}

bool BoardConst::get_piece_by_name(const string& name, Piece& piece) const
{
    for (unsigned i = 0; i < m_nu_pieces; ++i)
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
    if (! m_geometry.is_onboard(p))
        return false;
    for (unsigned i = 0; i < m_pieces.size(); ++i)
    {
        Piece piece(i);
        if (get_piece_info(piece).get_size() == points.size())
        {
            const Board::LocalMovesListRange& moves = get_moves(piece, p);
            for (auto j = moves.first; j != moves.second; ++j)
                if (m_move_info[j->to_int()].points == sorted_points)
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
    m_adj_status.init(m_geometry);
    array<bool, adj_status_nu_adj> forbidden;
    for (GeometryIterator i(m_geometry); i; ++i)
        init_adj_status(*i, forbidden, 0);
}

void BoardConst::init_adj_status(Point p,
                                 array<bool, adj_status_nu_adj>& forbidden,
                                 unsigned i)
{
    if (i == adj_status_nu_adj || i == m_geometry.get_adj_diag(p).size())
    {
        unsigned index = 0;
        for (unsigned j = 0; j < i; ++j)
            if (forbidden[j])
                index |= (1 << j);
        unsigned n = 0;
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
    for (unsigned i = 0; i < m_move_info.size(); ++i)
    {
        const MoveInfo& info = m_move_info[i];
        MoveInfoExt& info_ext = m_move_info_ext[i];
        MovePoints sym_points;
        info_ext.breaks_symmetry = false;
        BOOST_FOREACH(Point p, info.points)
        {
            if (info.points.contains(symmetric_points[p]))
                info_ext.breaks_symmetry = true;
            sym_points.push_back(symmetric_points[p]);
        }
        find_move(sym_points, info_ext.symmetric_move);
    }
}

bool BoardConst::is_compatible_with_adj_status(Point p,
                                               unsigned adj_status_index,
                                               const MovePoints& points) const
{
    BOOST_FOREACH(Point p_adj, m_adj_status[p][adj_status_index])
        if (points.contains(p_adj))
            return false;
    return true;
}

void BoardConst::set_adj_and_attach_points(const MoveInfo& info,
                                           MoveInfoExt& info_ext)
{
    auto begin = info.points.begin();
    auto end = info.points.end();
    m_marker.clear();
    for (auto i = begin; i != end; ++i)
        m_marker.set(*i);
    info_ext.adj_points.clear();
    for (auto i = begin; i != end; ++i)
        for (AdjIterator j(m_geometry, *i); j; ++j)
            if (m_geometry.is_onboard(*j) && ! m_marker[*j])
            {
                m_marker.set(*j);
                info_ext.adj_points.push_back(*j);
            }
    info_ext.attach_points.clear();
    for (auto i = begin; i != end; ++i)
        for (DiagIterator j(m_geometry, *i); j; ++j)
            if (m_geometry.is_onboard(*j) && ! m_marker[*j])
            {
                m_marker.set(*j);
                info_ext.attach_points.push_back(*j);
            }
    m_max_attach_points[info.piece] =
        max(m_max_attach_points[info.piece], info_ext.attach_points.size());
}

string BoardConst::to_string(Move mv, bool with_piece_name) const
{
    if (mv.is_null())
        return "null";
    if (mv.is_pass())
        return "pass";
    const MoveInfo& info = get_move_info(mv);
    ostringstream s;
    if (with_piece_name)
        s << '[' << get_piece_info(info.piece).get_name() << "]";
    bool is_first = true;
    BOOST_FOREACH(Point p, info.points)
    {
        if (! is_first)
            s << ',';
        else
            is_first = false;
        s << p;
    }
    return s.str();
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
