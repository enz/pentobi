//-----------------------------------------------------------------------------
/** @file libpentobi_thumbnail/CreateThumbnail.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "CreateThumbnail.h"

#include <iostream>
#include "libboardgame_base/RectGeometry.h"
#include "libboardgame_base/TrigonGeometry.h"
#include "libboardgame_sgf/SgfUtil.h"
#include "libboardgame_sgf/TreeReader.h"
#include "libboardgame_util/StringUtil.h"
#include "libpentobi_base/NodeUtil.h"
#include "libpentobi_gui/BoardPainter.h"

using namespace std;
using libboardgame_base::RectGeometry;
using libboardgame_base::TrigonGeometry;
using libboardgame_sgf::SgfNode;
using libboardgame_sgf::TreeReader;
using libboardgame_util::split;
using libboardgame_util::trim;
using libpentobi_base::Variant;
using libpentobi_base::Geometry;
using libpentobi_base::Grid;
using libpentobi_base::PointState;

//-----------------------------------------------------------------------------

namespace {

/** Helper function for getFinalPosition() */
void handleSetup(const char* id, Color c, const SgfNode& node,
                 const Geometry& geo, Grid<PointState>& pointState)
{
    vector<string> values = node.get_multi_property(id);
    for (const string& s : values)
    {
        if (trim(s).empty())
            continue;
        vector<string> v = split(s, ',');
        for (const string& p_str : v)
        {
            Point p;
            if (geo.from_string(p_str, p))
                pointState[p] = c;
        }
    }
}

/** Helper function for getFinalPosition() */
void handleSetupEmpty(const SgfNode& node, const Geometry& geo,
                      Grid<PointState>& pointState)
{
    vector<string> values = node.get_multi_property("AE");
    for (const auto& s : values)
    {
        if (trim(s).empty())
            continue;
        vector<string> v = split(s, ',');
        for (const auto& p_str : v)
        {
            Point p;
            if (geo.from_string(p_str, p))
                pointState[p] = PointState::empty();
        }
    }
}

/** Get the board state of the final position of the main variation.
    Avoids constructing an instance of a Tree or Game, which would do a costly
    initialization of BoardConst and slow down the thumbnailer
    unnecessarily. */
bool getFinalPosition(const SgfNode& root, Variant& variant, const Geometry*& geo,
                      Grid<PointState>& pointState)
{
    if (! parse_variant(root.get_property("GM", ""), variant))
        return false;
    geo = &get_geometry(variant);
    pointState.fill(PointState::empty(), *geo);
    auto node = &root;
    while (node)
    {
        if (libpentobi_base::node_util::has_setup(*node))
        {
            handleSetup("AB", Color(0), *node, *geo, pointState);
            handleSetup("AW", Color(1), *node, *geo, pointState);
            handleSetup("A1", Color(0), *node, *geo, pointState);
            handleSetup("A2", Color(1), *node, *geo, pointState);
            handleSetup("A3", Color(2), *node, *geo, pointState);
            handleSetup("A4", Color(3), *node, *geo, pointState);
            handleSetupEmpty(*node, *geo, pointState);
            if (node == &root)
                // If the file starts with a setup (e.g. a puzzle), we use this
                // position for the thumbnail.
                break;
        }
        Color c;
        MovePoints points;
        if (libpentobi_base::node_util::get_move(*node, variant, c, points))
            for (Point p : points)
                    pointState[p] = c;
        node = node->get_first_child_or_null();
    }
    return true;
}

} // namespace

//-----------------------------------------------------------------------------

bool createThumbnail(const QString& path, int width, int height,
                     QImage& image)
{
    TreeReader reader;
    reader.set_read_only_main_variation(true);
    reader.read(path.toLocal8Bit().constData());
    auto variant =
        Variant::classic; // Initialize to avoid compiler warning
    const Geometry* geo;
    Grid<PointState> pointState;
    if (! getFinalPosition(reader.get_tree(), variant, geo, pointState))
    {
        cerr << "Not a valid Blokus SGF file\n";
        return false;
    }

    BoardPainter boardPainter;
    QPainter painter;
    painter.begin(&image);
    boardPainter.paintEmptyBoard(painter, width, height, variant, *geo);
    boardPainter.paintPieces(painter, pointState);
    painter.end();
    return true;
}

//-----------------------------------------------------------------------------
