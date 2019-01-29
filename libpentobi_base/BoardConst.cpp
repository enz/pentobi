//-----------------------------------------------------------------------------
/** @file libpentobi_base/BoardConst.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "BoardConst.h"

#include <algorithm>
#include "Marker.h"
#include "PieceTransformsClassic.h"
#include "PieceTransformsGembloQ.h"
#include "PieceTransformsTrigon.h"
#include "libboardgame_util/Compiler.h"
#include "libboardgame_util/Log.h"

namespace libpentobi_base {

using libboardgame_util::get_type_name;

//-----------------------------------------------------------------------------

namespace {

const bool log_move_creation = false;

/** Local variable used during construction.
    Making this variable global slightly speeds up construction and a
    thread-safe construction is not needed. */
Marker g_marker;

/** Non-compact representation of lists of moves of a piece at a point
    constrained by the forbidden status of adjacent points.
    Only used during construction. See g_marker why this variable is global. */
Grid<array<ArrayList<Move, 44>, PrecompMoves::nu_adj_status>>
    g_full_move_table;


bool is_reverse(MovePoints::const_iterator begin1, const Point* begin2, unsigned size)
{
    auto j = begin2 + size - 1;
    for (auto i = begin1; i != begin1 + size; ++i, --j)
        if (*i != *j)
            return false;
    return true;
}

// Sort points using the ordering used in blksgf files (switches the direction
// of the y axis!)
void sort_piece_points(PiecePoints& points)
{
    auto less = [](CoordPoint a, CoordPoint b)
    {
        return ((a.y == b.y && a.x < b.x) || a.y > b.y);
    };
    auto check = [&](unsigned short a, unsigned short b)
    {
        if (! less(points[a], points[b]))
            swap(points[a], points[b]);
    };
    // Minimal number of necessary comparisons with sorting networks
    auto size = points.size();
    switch (size)
    {
    case 7:
        check(1, 2);
        check(3, 4);
        check(5, 6);
        check(0, 2);
        check(3, 5);
        check(4, 6);
        check(0, 1);
        check(4, 5);
        check(2, 6);
        check(0, 4);
        check(1, 5);
        check(0, 3);
        check(2, 5);
        check(1, 3);
        check(2, 4);
        check(2, 3);
        break;
    case 6:
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
        break;
    case 5:
        check(0, 1);
        check(3, 4);
        check(2, 4);
        check(2, 3);
        check(1, 4);
        check(0, 3);
        check(0, 2);
        check(1, 3);
        check(1, 2);
        break;
    case 4:
        check(0, 1);
        check(2, 3);
        check(0, 2);
        check(1, 3);
        check(1, 2);
        break;
    case 3:
        check(1, 2);
        check(0, 2);
        check(0, 1);
        break;
    case 2:
        check(0, 1);
        break;
    case 1:
        break;
    default:
        sort(points.begin(), points.end(), less);
    }
}

vector<PieceInfo> create_pieces_callisto(const Geometry& geo,
                                         const PieceTransforms& transforms)
{
    auto geometry_type = GeometryType::callisto;
    vector<PieceInfo> pieces;
    pieces.reserve(19);
    pieces.emplace_back("1",
                        PiecePoints{ CoordPoint(0, 0) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 3);
    pieces.emplace_back("W",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(-1, -1),
                                     CoordPoint(0, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("X",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, -1),
                                     CoordPoint(0, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 0) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("T5",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, -1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("U",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(-1, -1),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(1, -1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("L",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("T4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("Z",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("O",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0), CoordPoint(1, -1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("V",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("I",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("2",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    return pieces;
}

vector<PieceInfo> create_pieces_classic(const Geometry& geo,
                                        const PieceTransforms& transforms)
{
    auto geometry_type = GeometryType::classic;
    vector<PieceInfo> pieces;
    // Define the 21 standard pieces. The piece names are the standard names as
    // in http://blokusstrategy.com/?p=48. The default orientation is chosen
    // such that it resembles the letter.
    pieces.reserve(21);
    pieces.emplace_back("V5",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2), CoordPoint(1, 0),
                                     CoordPoint(2, 0) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("L5",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(1, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("Z5",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("N",
                        PiecePoints{ CoordPoint(-1, 1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2)},
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("W",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(-1, -1),
                                     CoordPoint(0, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("X",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, -1),
                                     CoordPoint(0, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 0) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("F",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(1, -1),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("I5",
                        PiecePoints{ CoordPoint(0, 2), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("T5",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, -1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("Y",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(0, 1),
                                     CoordPoint(0, 2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("P",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 0),
                                     CoordPoint(1, -1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("U",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(-1, -1),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(1, -1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("L4",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("I4",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(0, 2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("T4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("Z4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("O",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0), CoordPoint(1, -1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("V3",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("I3",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("2",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("1",
                        PiecePoints{ CoordPoint(0, 0) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    return pieces;
}

vector<PieceInfo> create_pieces_gembloq(const Geometry& geo,
                                        const PieceTransforms& transforms)
{
    auto geometry_type = GeometryType::gembloq;
    vector<PieceInfo> pieces;
    pieces.reserve(21);
    pieces.emplace_back("P",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(1, 1), CoordPoint(2, 1),
                                     CoordPoint(3, 1), CoordPoint(3, 2),
                                     CoordPoint(-3, -1), CoordPoint(-2, -1),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(1, -1), CoordPoint(2, -1),
                                     CoordPoint(-3, -2), CoordPoint(-2, -2),
                                     CoordPoint(-1, -2), CoordPoint(0, -2),
                                     CoordPoint(1, -2), CoordPoint(2, -2),
                                     CoordPoint(-1, -3), CoordPoint(0, -3) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("I5",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(-3, -1), CoordPoint(-2, -1),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(-5, -2), CoordPoint(-4, -2),
                                     CoordPoint(-3, -2), CoordPoint(-2, -2),
                                     CoordPoint(-5, -3), CoordPoint(-4, -3),
                                     CoordPoint(1, 1), CoordPoint(2, 1),
                                     CoordPoint(3, 1), CoordPoint(4, 1),
                                     CoordPoint(3, 2), CoordPoint(4, 2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("X",
                        PiecePoints{ CoordPoint(-3, 0), CoordPoint(-2, 0),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(-3, -1), CoordPoint(-2, -1),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(1, -1), CoordPoint(2, -1),
                                     CoordPoint(-3, -2), CoordPoint(-2, -2),
                                     CoordPoint(1, -2), CoordPoint(2, -2),
                                     CoordPoint(-3, 1), CoordPoint(-2, 1),
                                     CoordPoint(1, 1), CoordPoint(2, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("W",
                        PiecePoints{ CoordPoint(-5, 0), CoordPoint(-4, 0),
                                     CoordPoint(-3, 0), CoordPoint(-2, 0),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(3, 0), CoordPoint(4, 0),
                                     CoordPoint(-5, -1), CoordPoint(-4, -1),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(3, -1), CoordPoint(4, -1),
                                     CoordPoint(-3, 1), CoordPoint(-2, 1),
                                     CoordPoint(1, 1), CoordPoint(2, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("Z",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(-1, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(3, 0), CoordPoint(4, 0),
                                     CoordPoint(-5, 0), CoordPoint(-4, 0),
                                     CoordPoint(-5, -1), CoordPoint(-4, -1),
                                     CoordPoint(1, 1), CoordPoint(2, 1),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(3, -1), CoordPoint(4, -1),
                                     CoordPoint(-3, -1), CoordPoint(-2, -1),
                                     CoordPoint(-3, -2), CoordPoint(-2, -2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("Y",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(-3, 0),
                                     CoordPoint(-2, 0), CoordPoint(-1, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(-2, -1), CoordPoint(-1, -1),
                                     CoordPoint(0, -1), CoordPoint(-3, -1),
                                     CoordPoint(-3, -2), CoordPoint(-2, -2),
                                     CoordPoint(-3, 1), CoordPoint(-2, 1),
                                     CoordPoint(1, 1), CoordPoint(2, 1),
                                     CoordPoint(3, 1), CoordPoint(4, 1),
                                     CoordPoint(3, 2), CoordPoint(4, 2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("N5",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(-3, -1),
                                     CoordPoint(-2, -1), CoordPoint(-1, -1),
                                     CoordPoint(-4, -2), CoordPoint(-3, -2),
                                     CoordPoint(-5, -2), CoordPoint(-5, -3),
                                     CoordPoint(-2, -2), CoordPoint(-4, -3),
                                     CoordPoint(-3, 0), CoordPoint(-2, 0),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(-3, 1), CoordPoint(-2, 1),
                                     CoordPoint(-1, 1), CoordPoint(0, 1),
                                     CoordPoint(-1, 2), CoordPoint(0, 2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("T5",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(-5, 0),
                                     CoordPoint(-4, 0), CoordPoint(-1, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(1, 1), CoordPoint(2, 1),
                                     CoordPoint(-5, -1), CoordPoint(-4, -1),
                                     CoordPoint(-3, -1), CoordPoint(-2, -1),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(-3, -2), CoordPoint(-2, -2),
                                     CoordPoint(-1, -2), CoordPoint(0, -2),
                                     CoordPoint(-1, -3), CoordPoint(0, -3) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("L5",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(-1, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(-5, -2), CoordPoint(-4, -2),
                                     CoordPoint(-5, -3), CoordPoint(-4, -3),
                                     CoordPoint(3, 0), CoordPoint(4, 0),
                                     CoordPoint(1, 1), CoordPoint(2, 1),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(3, -1), CoordPoint(4, -1),
                                     CoordPoint(-3, -1), CoordPoint(-2, -1),
                                     CoordPoint(-3, -2), CoordPoint(-2, -2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("N4.5",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(-3, -1),
                                     CoordPoint(-2, -1), CoordPoint(-1, -1),
                                     CoordPoint(-4, -2), CoordPoint(-3, -2),
                                     CoordPoint(-2, -2), CoordPoint(-4, -3),
                                     CoordPoint(-3, 0), CoordPoint(-2, 0),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(-3, 1), CoordPoint(-2, 1),
                                     CoordPoint(-1, 1), CoordPoint(0, 1),
                                     CoordPoint(-1, 2), CoordPoint(0, 2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("O",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(-3, 0),
                                     CoordPoint(-2, 0), CoordPoint(-1, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(-3, 1), CoordPoint(-2, 1),
                                     CoordPoint(-1, 1), CoordPoint(0, 1),
                                     CoordPoint(1, 1), CoordPoint(2, 1),
                                     CoordPoint(-1, 2), CoordPoint(0, 2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("T4",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(-3, 0),
                                     CoordPoint(-2, 0), CoordPoint(-1, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(1, -1), CoordPoint(2, -1),
                                     CoordPoint(1, -2), CoordPoint(2, -2),
                                     CoordPoint(-3, 1), CoordPoint(-2, 1),
                                     CoordPoint(1, 1), CoordPoint(2, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("L4",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(-1, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(3, 0), CoordPoint(4, 0),
                                     CoordPoint(1, 1), CoordPoint(2, 1),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(3, -1), CoordPoint(4, -1),
                                     CoordPoint(-3, -1), CoordPoint(-2, -1),
                                     CoordPoint(-3, -2), CoordPoint(-2, -2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("I4",
                        PiecePoints{ CoordPoint(-2, -1), CoordPoint(-3, -1),
                                     CoordPoint(-3, -2), CoordPoint(-2, -2),
                                     CoordPoint(0, 0), CoordPoint(-1, 0),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(2, 1), CoordPoint(1, 1),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(4, 2), CoordPoint(3, 2),
                                     CoordPoint(3, 1), CoordPoint(4, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("L3.5",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(-1, 0),
                                     CoordPoint(-3, -1), CoordPoint(-2, -1),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(1, -1), CoordPoint(2, -1),
                                     CoordPoint(1, -2), CoordPoint(2, -2),
                                     CoordPoint(-5, -2), CoordPoint(-4, -2),
                                     CoordPoint(-3, -2), CoordPoint(-2, -2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("V",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(-1, 0),
                                     CoordPoint(-3, -1), CoordPoint(-2, -1),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(1, -1), CoordPoint(2, -1),
                                     CoordPoint(1, -2), CoordPoint(2, -2),
                                     CoordPoint(-3, -2), CoordPoint(-2, -2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("I3",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(-1, 0),
                                     CoordPoint(-3, -1), CoordPoint(-2, -1),
                                     CoordPoint(-3, -2), CoordPoint(-2, -2),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(1, 1), CoordPoint(2, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("L2.5",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(-1, 0),
                                     CoordPoint(-3, -1), CoordPoint(-2, -1),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(1, -1), CoordPoint(2, -1),
                                     CoordPoint(-3, -2), CoordPoint(-2, -2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("2",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(-1, 0),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(1, 1), CoordPoint(2, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("1.5",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(-1, 0),
                                     CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(1, 0), CoordPoint(2, 0)},
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("1",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(-1, 0),
                                     CoordPoint(-1, -1), CoordPoint(0, -1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    return pieces;
}

vector<PieceInfo> create_pieces_junior(const Geometry& geo,
                                       const PieceTransforms& transforms)
{
    auto geometry_type = GeometryType::classic;
    vector<PieceInfo> pieces;
    pieces.reserve(12);
    pieces.emplace_back("L5",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(1, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("P",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 0),
                                     CoordPoint(1, -1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("I5",
                        PiecePoints{ CoordPoint(0, 2), CoordPoint(0, 1),
                                     CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("O",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0), CoordPoint(1, -1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("T4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("Z4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("L4",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(1, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("I4",
                        PiecePoints{ CoordPoint(0, 1), CoordPoint(0, 0),
                                     CoordPoint(0, -1), CoordPoint(0, -2) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("V3",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(0, -1),
                                     CoordPoint(1, 0) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("I3",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("2",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    pieces.emplace_back("1",
                        PiecePoints{ CoordPoint(0, 0) },
                        geo, transforms, geometry_type, CoordPoint(0, 0), 2);
    return pieces;
}

// Note that the pieces for Trigon are currently used for both trigon_3 and
// the other Trigon variants even if the point types of their geometries are
// not compatible (e.g. whether the point with coordinates 0,0 is an upward or
// downward triangle). This requires special handling of Trigon at several
// places. In the future, we should probably use a separate set of Trigon
// pieces for even-sized and odd-sized boards instead.
vector<PieceInfo> create_pieces_trigon(const Geometry& geo,
                                       const PieceTransforms& transforms)
{
    auto geometry_type = GeometryType::trigon;
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
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("L6",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(2, -1),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms, geometry_type, CoordPoint(1, 0));
    pieces.emplace_back("V",
                        PiecePoints{ CoordPoint(-2, -1), CoordPoint(-1, -1),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("S",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(-1, 1), CoordPoint(0, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("P6",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(-1, 1), CoordPoint(0, 1) },
                        geo, transforms, geometry_type, CoordPoint(1, 0));
    pieces.emplace_back("F",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1),
                                     CoordPoint(2, 1), CoordPoint(1, 2) },
                        geo, transforms, geometry_type, CoordPoint(0, 1));
    pieces.emplace_back("W",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(2, 0), CoordPoint(3, 0) },
                        geo, transforms, geometry_type, CoordPoint(1, 0));
    pieces.emplace_back("A6",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(0, 1), CoordPoint(2, 1) },
                        geo, transforms, geometry_type, CoordPoint(1, 0));
    pieces.emplace_back("G",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 1), CoordPoint(2, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("Y",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(-1, 1), CoordPoint(0, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("X",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(-1, 1),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("O",
                        PiecePoints{ CoordPoint(-1, -1), CoordPoint(0, -1),
                                     CoordPoint(1, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(1, 0) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("I5",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(-1, 1),
                                     CoordPoint(0, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("L5",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("C5",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1),
                                     CoordPoint(2, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 1));
    pieces.emplace_back("P5",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, geometry_type, CoordPoint(1, 0));
    pieces.emplace_back("I4",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(-1, 1), CoordPoint(0, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("C4",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("A4",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0), CoordPoint(2, 0) },
                        geo, transforms, geometry_type, CoordPoint(1, 0));
    pieces.emplace_back("I3",
                        PiecePoints{ CoordPoint(1, -1), CoordPoint(0, 0),
                                     CoordPoint(1, 0) },
                        geo, transforms, geometry_type, CoordPoint(1, 0));
    pieces.emplace_back("2",
                        PiecePoints{ CoordPoint(0, 0), CoordPoint(1, 0) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    pieces.emplace_back("1",
                        PiecePoints{ CoordPoint(0, 0) },
                        geo, transforms, geometry_type, CoordPoint(0, 0));
    return pieces;
}

vector<PieceInfo> create_pieces_nexos(const Geometry& geo,
                                      const PieceTransforms& transforms)
{
    auto geometry_type = GeometryType::nexos;
    vector<PieceInfo> pieces;
    pieces.reserve(24);
    pieces.emplace_back("I4",
                        PiecePoints{ CoordPoint(0, -3), CoordPoint(0, -2),
                                     CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(0, 2),
                                     CoordPoint(0, 3) },
                        geo, transforms, geometry_type, CoordPoint(0, 1));
    pieces.emplace_back("L4",
                        PiecePoints{ CoordPoint(0, -3), CoordPoint(0, -2),
                                     CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 2) },
                        geo, transforms, geometry_type, CoordPoint(0, 1));
    pieces.emplace_back("Y",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 1), CoordPoint(0, 2),
                                     CoordPoint(0, 3)},
                        geo, transforms, geometry_type, CoordPoint(0, 1));
    pieces.emplace_back("N",
                        PiecePoints{ CoordPoint(-2, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 1), CoordPoint(0, 2),
                                     CoordPoint(0, 3)},
                        geo, transforms, geometry_type, CoordPoint(0, 1));
    pieces.emplace_back("V4",
                        PiecePoints{ CoordPoint(-3, 0), CoordPoint(-2, 0),
                                     CoordPoint(-1, 0), CoordPoint(0, -1),
                                     CoordPoint(0, -2), CoordPoint(0, -3) },
                        geo, transforms, geometry_type, CoordPoint(-1, 0));
    pieces.emplace_back("W",
                        PiecePoints{ CoordPoint(-2, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 2)},
                        geo, transforms, geometry_type, CoordPoint(-1, 0));
    pieces.emplace_back("Z4",
                        PiecePoints{ CoordPoint(-1, -2), CoordPoint(0, -1),
                                     CoordPoint(0, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 2) },
                        geo, transforms, geometry_type, CoordPoint(0, 1));
    pieces.emplace_back("T4",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(0, 2),
                                     CoordPoint(0, 3) },
                        geo, transforms, geometry_type, CoordPoint(0, 1));
    pieces.emplace_back("E",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(-1, 2)},
                        geo, transforms, geometry_type, CoordPoint(0, 1));
    pieces.emplace_back("U4",
                        PiecePoints{ CoordPoint(-2, -1), CoordPoint(-1, 0),
                                     CoordPoint(0, 0), CoordPoint(1, 0),
                                     CoordPoint(2, -1) },
                        geo, transforms, geometry_type, CoordPoint(-1, 0));
    pieces.emplace_back("X",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(-1, 0),
                                     CoordPoint(1, 0), CoordPoint(0, 1)},
                        geo, transforms, geometry_type, CoordPoint(0, -1));
    pieces.emplace_back("F",
                        PiecePoints{ CoordPoint(1, -2), CoordPoint(0, -1),
                                     CoordPoint(1, 0), CoordPoint(0, 1)},
                        geo, transforms, geometry_type, CoordPoint(0, -1));
    pieces.emplace_back("H",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(2, 1)},
                        geo, transforms, geometry_type, CoordPoint(0, 1));
    pieces.emplace_back("J",
                        PiecePoints{ CoordPoint(0, -3), CoordPoint(0, -2),
                                     CoordPoint(0, -1), CoordPoint(-1, 0),
                                     CoordPoint(-2, -1) },
                        geo, transforms, geometry_type, CoordPoint(-1, 0));
    pieces.emplace_back("G",
                        PiecePoints{ CoordPoint(2, -1), CoordPoint(1, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 2)},
                        geo, transforms, geometry_type, CoordPoint(1, 0));
    pieces.emplace_back("O",
                        PiecePoints{ CoordPoint(1, 0), CoordPoint(2, 1),
                                     CoordPoint(0, 1), CoordPoint(1, 2)},
                        geo, transforms, geometry_type, CoordPoint(0, 1));
    pieces.emplace_back("I3",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(0, 2),
                                     CoordPoint(0, 3) },
                        geo, transforms, geometry_type, CoordPoint(0, 1));
    pieces.emplace_back("L3",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1), CoordPoint(1, 2) },
                        geo, transforms, geometry_type, CoordPoint(0, 1));
    pieces.emplace_back("T3",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(1, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 1));
    pieces.emplace_back("Z3",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, 1),
                                     CoordPoint(1, 2) },
                        geo, transforms, geometry_type, CoordPoint(0, 1));
    pieces.emplace_back("U3",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(1, 0),
                                     CoordPoint(2, -1) },
                        geo, transforms, geometry_type, CoordPoint(1, 0));
    pieces.emplace_back("V2",
                        PiecePoints{ CoordPoint(-1, 0), CoordPoint(0, -1) },
                        geo, transforms, geometry_type, CoordPoint(-1, 0));
    pieces.emplace_back("I2",
                        PiecePoints{ CoordPoint(0, -1), CoordPoint(0, 0),
                                     CoordPoint(0, 1) },
                        geo, transforms, geometry_type, CoordPoint(0, 1));
    pieces.emplace_back("1",
                        PiecePoints{ CoordPoint(1, 0) },
                        geo, transforms, geometry_type, CoordPoint(1, 0));
    return pieces;
}

} // namespace

//-----------------------------------------------------------------------------

BoardConst::BoardConst(BoardType board_type, PieceSet piece_set)
    : m_board_type(board_type),
      m_piece_set(piece_set),
      m_geo(libpentobi_base::get_geometry(board_type))
{
    switch (board_type)
    {
    case BoardType::classic:
        m_range = Move::onboard_moves_classic;
        break;
    case BoardType::trigon:
        m_range = Move::onboard_moves_trigon;
        break;
    case BoardType::trigon_3:
        m_range = Move::onboard_moves_trigon_3;
        break;
    case BoardType::duo:
        if (piece_set == PieceSet::classic)
            m_range = Move::onboard_moves_duo;
        else
        {
            LIBBOARDGAME_ASSERT(piece_set == PieceSet::junior);
            m_range = Move::onboard_moves_junior;
        }
        break;
    case BoardType::nexos:
        m_range = Move::onboard_moves_nexos;
        break;
    case BoardType::callisto:
        m_range = Move::onboard_moves_callisto;
        break;
    case BoardType::callisto_2:
        m_range = Move::onboard_moves_callisto_2;
        break;
    case BoardType::callisto_3:
        m_range = Move::onboard_moves_callisto_3;
        break;
    case BoardType::gembloq:
        m_range = Move::onboard_moves_gembloq;
        break;
    case BoardType::gembloq_2:
        m_range = Move::onboard_moves_gembloq_2;
        break;
    case BoardType::gembloq_3:
        m_range = Move::onboard_moves_gembloq_3;
        break;
    }
    ++m_range; // Move::null()
    switch (piece_set)
    {
    case PieceSet::classic:
        m_transforms = make_unique<PieceTransformsClassic>();
        m_pieces = create_pieces_classic(m_geo, *m_transforms);
        m_max_piece_size = 5;
        m_max_adj_attach = 16;
        m_move_info.reset(calloc(m_range, sizeof(MoveInfo<5>)));
        m_move_info_ext.reset(calloc(m_range, sizeof(MoveInfoExt<16>)));
        break;
    case PieceSet::junior:
        m_transforms = make_unique<PieceTransformsClassic>();
        m_pieces = create_pieces_junior(m_geo, *m_transforms);
        m_max_piece_size = 5;
        m_max_adj_attach = 16;
        m_move_info.reset(calloc(m_range, sizeof(MoveInfo<5>)));
        m_move_info_ext.reset(calloc(m_range, sizeof(MoveInfoExt<16>)));
        break;
    case PieceSet::trigon:
        m_transforms = make_unique<PieceTransformsTrigon>();
        m_pieces = create_pieces_trigon(m_geo, *m_transforms);
        m_max_piece_size = 6;
        m_max_adj_attach = 22;
        m_move_info.reset(calloc(m_range, sizeof(MoveInfo<6>)));
        m_move_info_ext.reset(calloc(m_range, sizeof(MoveInfoExt<22>)));
        break;
    case PieceSet::nexos:
        m_transforms = make_unique<PieceTransformsClassic>();
        m_pieces = create_pieces_nexos(m_geo, *m_transforms);
        m_max_piece_size = 7;
        m_max_adj_attach = 12;
        m_move_info.reset(calloc(m_range, sizeof(MoveInfo<7>)));
        m_move_info_ext.reset(calloc(m_range, sizeof(MoveInfoExt<12>)));
        break;
    case PieceSet::callisto:
        m_transforms = make_unique<PieceTransformsClassic>();
        m_pieces = create_pieces_callisto(m_geo, *m_transforms);
        m_max_piece_size = 5;
        // m_max_adj_attach is actually 10 in Callisto, but we care more about
        // the performance in the classic Blokus variants and some code is
        // faster if we don't have to handle different values for
        // m_max_adj_attach for the same m_max_piece_size.
        m_max_adj_attach = 16;
        m_move_info.reset(calloc(m_range, sizeof(MoveInfo<5>)));
        m_move_info_ext.reset(calloc(m_range, sizeof(MoveInfoExt<16>)));
        break;
    case PieceSet::gembloq:
        m_transforms = make_unique<PieceTransformsGembloQ>();
        m_pieces = create_pieces_gembloq(m_geo, *m_transforms);
        m_max_piece_size = 22;
        m_max_adj_attach = 44;
        m_move_info.reset(calloc(m_range, sizeof(MoveInfo<22>)));
        m_move_info_ext.reset(calloc(m_range, sizeof(MoveInfoExt<44>)));
        break;
    }
    m_move_info_ext_2 = make_unique<MoveInfoExt2[]>(m_range);
    m_nu_pieces = static_cast<Piece::IntType>(m_pieces.size());
    for (Point p : m_geo)
        if (has_adj_status_points(p))
            init_adj_status_points(p);
    auto width = m_geo.get_width();
    auto height = m_geo.get_height();
    for (Point p : m_geo)
        m_compare_val[p] =
                (height - m_geo.get_y(p) - 1) * width + m_geo.get_x(p);
    create_moves();
    switch (piece_set)
    {
    case PieceSet::classic:
        LIBBOARDGAME_ASSERT(m_nu_pieces == 21);
        break;
    case PieceSet::junior:
        LIBBOARDGAME_ASSERT(m_nu_pieces == 12);
        break;
    case PieceSet::trigon:
        LIBBOARDGAME_ASSERT(m_nu_pieces == 22);
        break;
    case PieceSet::nexos:
        LIBBOARDGAME_ASSERT(m_nu_pieces == 24);
        break;
    case PieceSet::callisto:
        LIBBOARDGAME_ASSERT(m_nu_pieces == 12);
        break;
    case PieceSet::gembloq:
        LIBBOARDGAME_ASSERT(m_nu_pieces == 21);
        break;
    }
    if (board_type == BoardType::duo || board_type == BoardType::callisto_2)
        init_symmetry_info<5>();
    else if (board_type == BoardType::trigon)
        init_symmetry_info<6>();
    else if (board_type == BoardType::gembloq_2)
        init_symmetry_info<22>();
}

template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
inline void BoardConst::create_move(unsigned& moves_created, Piece piece,
                                    const MovePoints& points, Point label_pos)
{
    LIBBOARDGAME_ASSERT(m_max_piece_size == MAX_SIZE);
    LIBBOARDGAME_ASSERT(m_max_adj_attach == MAX_ADJ_ATTACH);
    LIBBOARDGAME_ASSERT(moves_created < m_range);
    Move mv(static_cast<Move::IntType>(moves_created));
    void* place =
            static_cast<MoveInfo<MAX_SIZE>*>(m_move_info.get())
            + moves_created;
    new(place) MoveInfo<MAX_SIZE>(piece, points);
    place =
            static_cast<MoveInfoExt<MAX_ADJ_ATTACH>*>(m_move_info_ext.get())
            + moves_created;
    auto& info_ext = *new(place) MoveInfoExt<MAX_ADJ_ATTACH>();
    auto& info_ext_2 = m_move_info_ext_2[moves_created];
    ++moves_created;
    auto scored_points = &info_ext_2.scored_points[0];
    for (auto p : points)
        if (m_board_type != BoardType::nexos || m_geo.get_point_type(p) != 0)
            *(scored_points++) = p;
    info_ext_2.scored_points_size = static_cast<uint_least8_t>(
                scored_points - &info_ext_2.scored_points[0]);
    auto begin = info_ext_2.begin_scored_points();
    auto end = info_ext_2.end_scored_points();
    g_marker.clear();
    for (auto i = begin; i != end; ++i)
        g_marker.set(*i);
    for (auto i = begin; i != end; ++i)
    {
        LIBBOARDGAME_ASSERT(has_adj_status_points(*i));
        auto j = m_adj_status_points[*i].begin();
        unsigned adj_status = g_marker[*j];
        for (unsigned k = 1; k < PrecompMoves::adj_status_nu_adj; ++k)
            adj_status |= (g_marker[*(++j)] << k);
        for (unsigned j = 0; j < PrecompMoves::nu_adj_status; ++j)
            if ((j & adj_status) == 0)
                g_full_move_table[*i][j].push_back(mv);
    }
    Point* p = info_ext.points;
    for (auto i = begin; i != end; ++i)
        for (Point j : m_geo.get_adj(*i))
            if (! g_marker[j])
            {
                g_marker.set(j);
                *(p++) = j;
            }
    info_ext.size_adj_points = static_cast<uint_least8_t>(p - info_ext.points);
    for (auto i = begin; i != end; ++i)
        for (Point j : m_geo.get_diag(*i))
            if (! g_marker[j])
            {
                g_marker.set(j);
                *(p++) = j;
            }
    info_ext.size_attach_points =
            static_cast<uint_least8_t>(p - info_ext.end_adj());
    info_ext_2.label_pos = label_pos;
    info_ext_2.breaks_symmetry = false;
    info_ext_2.symmetric_move = Move::null();
    m_nu_attach_points[piece] =
        max(m_nu_attach_points[piece],
            static_cast<unsigned>(info_ext.size_attach_points));
    if (log_move_creation)
    {
        Grid<char> grid;
        grid.fill('.', m_geo);
        for (auto i = begin; i != end; ++i)
            grid[*i] = 'O';
        for (auto i = info_ext.begin_adj(); i != info_ext.end_adj(); ++i)
            grid[*i] = '+';
        for (auto i = info_ext.begin_attach(); i != info_ext.end_attach(); ++i)
            grid[*i] = '*';
        LIBBOARDGAME_LOG("Move ", mv.to_int(), ":\n", grid.to_string(m_geo));
    }
}

void BoardConst::create_moves()
{
    // Unused move infos for Move::null()
    LIBBOARDGAME_ASSERT(Move::null().to_int() == 0);
    unsigned moves_created = 1;

    unsigned n = 0;
    for (Piece::IntType i = 0; i < m_nu_pieces; ++i)
    {
        Piece piece(i);
        if (m_max_piece_size == 5)
            create_moves<5, 16>(moves_created, piece);
        else if (m_max_piece_size == 6)
            create_moves<6, 22>(moves_created, piece);
        else if (m_max_piece_size == 7)
            create_moves<7, 12>(moves_created, piece);
        else
            create_moves<22, 44>(moves_created, piece);
        for (Point p : m_geo)
            for (unsigned j = 0; j < PrecompMoves::nu_adj_status; ++j)
                {
                    auto& list = g_full_move_table[p][j];
                    m_precomp_moves.set_list_range(p, j, piece, n,
                                                   list.size());
                    for (auto mv : list)
                        m_precomp_moves.set_move(n++, mv);
                    list.clear();
                }
    }
    LIBBOARDGAME_ASSERT(moves_created == m_range);
    LIBBOARDGAME_LOG("Created moves: ", moves_created, ", precomp: ", n);
}

template<unsigned MAX_SIZE, unsigned MAX_ADJ_ATTACH>
void BoardConst::create_moves(unsigned& moves_created, Piece piece)
{
    auto& piece_info = m_pieces[piece.to_int()];
    if (log_move_creation)
        LIBBOARDGAME_LOG("Creating moves for piece ", piece_info.get_name());
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
    auto piece_size =
            static_cast<MovePoints::IntType>(piece_info.get_points().size());
    MovePoints points;
    for (MovePoints::IntType i = 0; i < MovePoints::max_size; ++i)
        points.get_unchecked(i) = Point::null();
    points.resize(piece_size);
    // Make outer loop iterator over geometry for better memory locality
    for (Point p : m_geo)
    {
        if (log_move_creation)
            LIBBOARDGAME_LOG("Creating moves at ", m_geo.to_string(p));
        auto x = static_cast<int>(m_geo.get_x(p));
        auto y = static_cast<int>(m_geo.get_y(p));
        auto point_type = m_geo.get_point_type(p);
        for (size_t i = 0; i < nu_transforms; ++i)
        {
            if (log_move_creation)
            {
#ifndef LIBBOARDGAME_DISABLE_LOG
                auto& transform = *transforms[i];
                LIBBOARDGAME_LOG("Transformation ", get_type_name(transform));
#endif
            }
            if (transforms[i]->get_point_type() != point_type)
                continue;
            bool is_onboard = true;
            for (MovePoints::IntType j = 0; j < piece_size; ++j)
            {
                auto& pp = transformed_points[i][j];
                int xx = pp.x + x;
                int yy = pp.y + y;
                if (! m_geo.is_onboard(xx, yy))
                {
                    is_onboard = false;
                    break;
                }
                points[j] = m_geo.get_point(xx, yy);
            }
            if (! is_onboard)
                continue;
            CoordPoint label_pos = transformed_label_pos[i];
            label_pos.x += x;
            label_pos.y += y;
            create_move<MAX_SIZE, MAX_ADJ_ATTACH>(
                        moves_created, piece, points,
                        m_geo.get_point(label_pos.x, label_pos.y));
        }
    }
}

bool BoardConst::from_string(Move& mv, const string& s) const
{
    if (s == "null")
    {
        mv = Move::null();
        return true;
    }
    MovePoints points;
    auto begin = s.begin();
    auto end = begin;
    while (true)
    {
        while (end != s.end() && *end != ',')
            ++end;
        Point p;
        if (! m_geo.from_string(begin, end, p))
            return false;
        if (points.size() == MovePoints::max_size)
            return false;
        points.push_back(p);
        if (end == s.end())
            break;
        ++end;
        begin = end;
    }
    return find_move(points, mv);
}

const BoardConst& BoardConst::get(Variant variant)
{
    static map<BoardType, map<PieceSet, unique_ptr<BoardConst>>> board_const;
    auto board_type = libpentobi_base::get_board_type(variant);
    auto piece_set = libpentobi_base::get_piece_set(variant);
    auto& bc = board_const[board_type][piece_set];
    if (! bc)
        bc.reset(new BoardConst(board_type, piece_set));
    return *bc;
}

Piece BoardConst::get_move_piece(Move mv) const
{
    if (m_max_piece_size == 5)
        return get_move_piece<5>(mv);
    if (m_max_piece_size == 6)
        return get_move_piece<6>(mv);
    if (m_max_piece_size == 7)
        return get_move_piece<7>(mv);
    LIBBOARDGAME_ASSERT(m_max_piece_size == 22);
    return get_move_piece<22>(mv);
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
    if (points.empty())
        return false;
    MovePoints sorted_points = points;
    sort(sorted_points);
    for (Piece::IntType i = 0; i < m_pieces.size(); ++i)
    {
        Piece piece(i);
        for (auto mv : get_moves(piece, points[0]))
        {
            auto& info_ext_2 = get_move_info_ext_2(mv);
            if (equal(sorted_points.begin(), sorted_points.end(),
                      info_ext_2.begin_scored_points(),
                      info_ext_2.end_scored_points()))
            {
                move = mv;
                return true;
            }
        }
    }
    return false;
}

bool BoardConst::find_move(const MovePoints& points, Piece piece,
                           Move& move) const
{
    MovePoints sorted_points = points;
    sort(sorted_points);
    for (auto mv : get_moves(piece, points[0]))
        if (equal(sorted_points.begin(), sorted_points.end(),
                  get_move_points_begin(mv)))
        {
            move = mv;
            return true;
        }
    return false;
}

/** Builds the list of neighboring points that is used for the adjacent
    status for matching precompted move lists. */
void BoardConst::init_adj_status_points(Point p)
{
    // The order of points affects the size of the precomputed lists. The
    // following algorithm does well but is not optimal for all geometries.
    auto& points = m_adj_status_points[p];
    const auto max_size = PrecompMoves::adj_status_nu_adj;
    unsigned n = 0;
    auto add_adj = [&](Point p)
    {
        for (Point pp : m_geo.get_adj(p))
        {
            if (n == max_size)
                return;
            auto end = points.begin() + n;
            if (find(points.begin(), end, pp) == end)
                points[n++] = pp;
        }
    };
    auto add_diag = [&](Point p)
    {
        for (Point pp : m_geo.get_diag(p))
        {
            if (n == max_size)
                return;
            auto end = points.begin() + n;
            if (find(points.begin(), end, pp) == end)
                points[n++] = pp;
        }
    };
    add_adj(p);
    add_diag(p);
    auto old_n = n;
    if (n < max_size)
    {
        for (unsigned i = 0; i < old_n; ++i)
        {
            add_adj(points[i]);
            if (n == max_size)
                break;
        }
    }
    if (n < max_size)
    {
        for (unsigned i = 0; i < old_n; ++i)
        {
            add_diag(points[i]);
            if (n == max_size)
                break;
        }
    }

    LIBBOARDGAME_ASSERT(n == max_size);
}

template<unsigned MAX_SIZE>
void BoardConst::init_symmetry_info()
{
    m_symmetric_points.init(m_geo);
    for (Move::IntType i = 1; i < m_range; ++i)
    {
        Move mv(i);
        auto& info = get_move_info<MAX_SIZE>(mv);
        auto& info_ext_2 = m_move_info_ext_2[i];
        info_ext_2.breaks_symmetry = false;
        array<Point, PieceInfo::max_size> sym_points;
        MovePoints::IntType n = 0;
        for (Point p : info)
        {
            auto symm_p = m_symmetric_points[p];
            auto end = info.end();
            if (find(info.begin(), end, symm_p) != end)
                info_ext_2.breaks_symmetry = true;
            sym_points[n++] = symm_p;
        }
        for (auto mv : get_moves(info.get_piece(), sym_points[0]))
            if (is_reverse(sym_points.begin(),
                           get_move_info<MAX_SIZE>(mv).begin(), n))
            {
                info_ext_2.symmetric_move = mv;
                break;
            }
    }
}

void BoardConst::sort(MovePoints& points) const
{
    auto less = [this](Point a, Point b)
    {
        return this->m_compare_val[a] < this->m_compare_val[b];
    };
    auto check = [&](unsigned short a, unsigned short b)
    {
        if (! less(points[a], points[b]))
            swap(points[a], points[b]);
    };
    // Minimal number of necessary comparisons with sorting networks
    auto size = points.size();
    switch (size)
    {
    case 7:
        check(1, 2);
        check(3, 4);
        check(5, 6);
        check(0, 2);
        check(3, 5);
        check(4, 6);
        check(0, 1);
        check(4, 5);
        check(2, 6);
        check(0, 4);
        check(1, 5);
        check(0, 3);
        check(2, 5);
        check(1, 3);
        check(2, 4);
        check(2, 3);
        break;
    case 6:
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
        break;
    case 5:
        check(0, 1);
        check(3, 4);
        check(2, 4);
        check(2, 3);
        check(1, 4);
        check(0, 3);
        check(0, 2);
        check(1, 3);
        check(1, 2);
        break;
    case 4:
        check(0, 1);
        check(2, 3);
        check(0, 2);
        check(1, 3);
        check(1, 2);
        break;
    case 3:
        check(1, 2);
        check(0, 2);
        check(0, 1);
        break;
    case 2:
        check(0, 1);
        break;
    case 1:
        break;
    default:
        std::sort(points.begin(), points.end(), less);
    }
}

string BoardConst::to_string(Move mv, bool with_piece_name) const
{
    if (mv.is_null())
        return "null";
    auto& info_ext_2 = get_move_info_ext_2(mv);
    ostringstream s;
    if (with_piece_name)
        s << '[' << get_piece_info(get_move_piece(mv)).get_name() << "]";
    bool is_first = true;
    for (auto i = info_ext_2.begin_scored_points();
         i != info_ext_2.end_scored_points(); ++i)
    {
        if (! is_first)
            s << ',';
        else
            is_first = false;
        s << m_geo.to_string(*i);
    }
    return s.str();
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base
