//-----------------------------------------------------------------------------
/** @file libpentobi_gui/PieceSelector.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PieceSelector.h"

#include <algorithm>
#include <QMouseEvent>
#include <QPainter>
#include "libboardgame_base/GeometryUtil.h"
#include "libboardgame_util/StringUtil.h"
#include "libpentobi_gui/Util.h"

using namespace std;
using libboardgame_base::CoordPoint;
using libboardgame_base::geometry_util::type_match_shift;
using libboardgame_util::trim;
using libpentobi_base::BoardConst;
using libpentobi_base::BoardType;
using libpentobi_base::Geometry;
using libpentobi_base::PieceMap;
using libpentobi_base::PieceSet;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

namespace {

const string pieceLayoutCallisto =
    " 1 . U U U . O O . O O . L L . L . Z . . Z . . I . I . 2"
    " . . U . U . O O . O O . L . . L . Z Z . Z Z . I . I . 2"
    " 1 . . . . . . . . . . . L . L L . . Z . . Z . I . I . ."
    " . .T5T5T5 . . W . . X . . . . . . . . . . . . . . . . 2"
    " 1 . .T5 . . W W . X X X .T4T4T4 .T4T4T4 . V . . V . . 2"
    " . . .T5 . W W . . . X . . .T4 . . .T4 . . V V . V V . .";

const string pieceLayoutClassic =
    " 1 .Z4Z4 . .L4L4L4 . O O . P P .L5L5L5L5 .V5V5V5 . U U U . N . . ."
    " . . .Z4Z4 . . .L4 . O O . P P .L5 . . . .V5 . . . U . U . N N .I5"
    " 2 2 . . . .T4 . . . . . . P . . . . X . .V5 .Z5 . . . . . . N .I5"
    " . . .I3 .T4T4T4 . . W W . . . F . X X X . . .Z5Z5Z5 . .T5 . N .I5"
    "V3 . .I3 . . . . . . . W W . F F . . X . . Y . . .Z5 . .T5 . . .I5"
    "V3V3 .I3 . .I4I4I4I4 . . W . . F F . . . Y Y Y Y . . .T5T5T5 . .I5";

const string pieceLayoutJunior =
    "1 . 1 . V3V3. . L4L4L4. T4T4T4. . O O . O O . P P . . I5. I5. . L5L5"
    ". . . . V3. . . L4. . . . T4. . . O O . O O . P P . . I5. I5. . . L5"
    "2 . 2 . . . V3. . . . L4. . . T4. . . . . . . P . . . I5. I5. L5. L5"
    "2 . 2 . . V3V3. . L4L4L4. . T4T4T4. . Z4. Z4. . . P . I5. I5. L5. L5"
    ". . . . . . . . . . . . . . . . . . Z4Z4. Z4Z4. P P . I5. I5. L5. . "
    "I3I3I3. I3I3I3. I4I4I4I4. I4I4I4I4. Z4. . . Z4. P P . . . . . L5L5. ";

const string pieceLayoutTrigon =
    "L5L5 . . F F F F . .L6L6 . . O O O . . X X X . . .A6A6 . . G G . G . .C4C4 . . Y Y Y Y"
    "L5L5 . . F . F . . .L6L6 . . O O O . . X X X . .A6A6A6A6 . . G G G . .C4C4 . . Y Y . ."
    " .L5 . . . . . . S . .L6L6 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 2"
    " . . . . . S S S S . . . . . . . .P5P5P5P5 . . .I6I6 . .I5I5I5I5I5 . . W W W W W . . 2"
    "C5C5 . . . S . . . . V V . .P6 . . . .P5 . .A4 . .I6I6 . . . . . . . . . . W . . . . ."
    "C5C5C5 . . . . V V V V . .P6P6P6P6P6 . . .A4A4A4 . .I6I6 . .I3I3I3 . . 1 . . .I4I4I4I4";

// To increase the clickable area and to ensure that the pieces can be found
// in the string with a flood-filling algorithm, the nexos pieces also include
// some crossable junction points that are not part of the piece definition
// (they will be filtered out before finding the piece). But the number of
// points per piece must be at most PiecePoints::max_size.
const string pieceLayoutNexos =
    " . . F F F F F . . . O O O .U4U4U4U4U4 . . . . N N N N . . . . H H H . .U3 .U3 . . .V2V2V2"
    "I4 . . . F . F . Y . O . O .U4 . . .U4 .T4 . . . . . N . . . . . H . . .U3 .U3 . . . . .V2"
    "I4 . . . . . . . Y . . O O . . . . . . .T4T4T4T4 . . N N . . . . H H . .U3U3U3 . . . . .V2"
    "I4 .L4 . . . . . Y . . . . . . . . . . .T4 . . . . . . . . . X . . . . . . . . . . . J . ."
    "I4 .L4 . . . . Y Y .L3 . G G . . . . . . . . . . .Z3Z3 . . X X X . . . . . .I2 . . . J . ."
    "I4 .L4 . W . . . Y .L3 . G . . . E . . . . .T3 . . .Z3 . . . X . . . . .Z4 .I2 . J . J .V4"
    "I4 .L4 . W W W . Y .L3 . G G G . E E E E . .T3T3 . .Z3Z3 . . . .Z4Z4Z4Z4Z4 .I2 . J J J .V4"
    "I4 .L4 . . . W . . .L3 . . . G . . . E . . .T3 . . . . . . . . .Z4 . . . . .I2 . . . . .V4"
    " . .L4L4 . . W W . .L3L3L3 . . . . . . . . . . . .I3I3I3I3I3 . . . . 1 1 1 .I2 . .V4V4V4V4";

} // namespace

//-----------------------------------------------------------------------------

PieceSelector::PieceSelector(QWidget* parent, const Board& bd, Color color)
    : QWidget(parent),
      m_bd(bd),
      m_color(color)
{
    setMinimumSize(170, 30);
    init();
}

void PieceSelector::checkUpdate()
{
    bool disabledStatus[maxColumns][maxRows];
    setDisabledStatus(disabledStatus);
    for (unsigned x = 0; x < m_nuColumns; ++x)
        for (unsigned y = 0; y < m_nuRows; ++y)
            if (! m_piece[x][y].is_null()
                    && disabledStatus[x][y] != m_disabled[x][y])
            {
                update();
                return;
            }
}

void PieceSelector::filterCrossableJunctions(PiecePoints& points) const
{
    auto& geo = m_bd.get_geometry();
    PiecePoints newPoints;
    for (auto& p : points)
    {
        if (geo.get_point_type(p.x, p.y) != 0)
            // Not a junction
            newPoints.push_back(p);
        else if (points.contains(CoordPoint(p.x - 1, p.y))
                 && points.contains(CoordPoint(p.x + 1, p.y))
                 && ! points.contains(CoordPoint(p.x, p.y - 1))
                 && ! points.contains(CoordPoint(p.x, p.y + 1)))
            // Necessary junction
            newPoints.push_back(p);
        else if (! points.contains(CoordPoint(p.x - 1, p.y))
                 && ! points.contains(CoordPoint(p.x + 1, p.y))
                 && points.contains(CoordPoint(p.x, p.y - 1))
                 && points.contains(CoordPoint(p.x, p.y + 1)))
            // Necessary junction
            newPoints.push_back(p);
    }
    points = newPoints;
}

void PieceSelector::findPiecePoints(Piece piece, unsigned x, unsigned y,
                                    PiecePoints& points) const
{
    CoordPoint p(x, y);
    if (x >= m_nuColumns || y >= m_nuRows || m_piece[x][y] != piece
            || points.contains(p))
        return;
    points.push_back(p);
    // This assumes that no Trigon pieces touch at the corners, otherwise
    // we would need to iterate over neighboring CoordPoint's corresponding to
    // Geometry::get_adj()
    findPiecePoints(piece, x + 1, y, points);
    findPiecePoints(piece, x - 1, y, points);
    findPiecePoints(piece, x, y + 1, points);
    findPiecePoints(piece, x, y - 1, points);
}

void PieceSelector::init()
{
    const string* pieceLayout = nullptr; // Init to avoid compiler warning
    auto pieceSet = m_bd.get_piece_set();
    switch (pieceSet)
    {
    case PieceSet::classic:
        pieceLayout = &pieceLayoutClassic;
        m_nuColumns = 33;
        m_nuRows = 6;
        break;
    case PieceSet::trigon:
        pieceLayout = &pieceLayoutTrigon;
        m_nuColumns = 43;
        m_nuRows = 6;
        break;
    case PieceSet::junior:
        pieceLayout = &pieceLayoutJunior;
        m_nuColumns = 34;
        m_nuRows = 6;
        break;
    case PieceSet::nexos:
        pieceLayout = &pieceLayoutNexos;
        m_nuColumns = 45;
        m_nuRows = 9;
        break;
    case PieceSet::callisto:
        pieceLayout = &pieceLayoutCallisto;
        m_nuColumns = 28;
        m_nuRows = 6;
        break;
    }
    LIBBOARDGAME_ASSERT(m_nuColumns <= maxColumns);
    LIBBOARDGAME_ASSERT(m_nuRows <= maxRows);
    for (unsigned y = 0; y < m_nuRows; ++y)
        for (unsigned x = 0; x < m_nuColumns; ++x)
        {
            string name = pieceLayout->substr(y * m_nuColumns * 2 + x * 2, 2);
            name = trim(name);
            Piece piece = Piece::null();
            if (name != ".")
            {
                m_bd.get_piece_by_name(name, piece);
                LIBBOARDGAME_ASSERT(! piece.is_null());
            }
            m_piece[x][y] = piece;
        }
    auto& geo = m_bd.get_geometry();
    for (unsigned y = 0; y < m_nuRows; ++y)
        for (unsigned x = 0; x < m_nuColumns; ++x)
        {
            Piece piece = m_piece[x][y];
            if (piece.is_null())
                continue;
            PiecePoints points;
            findPiecePoints(piece, x, y, points);
            // We need to match the coordinate system of the piece selector to
            // the geometry, they are different in Trigon3.
            type_match_shift(geo, points.begin(), points.end(), 0);
            if (pieceSet == PieceSet::nexos)
                filterCrossableJunctions(points);
            m_transform[x][y] =
                m_bd.get_piece_info(piece).find_transform(geo, points);
            LIBBOARDGAME_ASSERT(m_transform[x][y]);
        }
    setDisabledStatus(m_disabled);
    update();
}

void PieceSelector::mousePressEvent(QMouseEvent* event)
{
    qreal pixelX = event->x() - 0.5 * (width() - m_selectorWidth);
    qreal pixelY = event->y() - 0.5 * (height() - m_selectorHeight);
    if (pixelX < 0 || pixelX >= m_selectorWidth
        || pixelY < 0 || pixelY >= m_selectorHeight)
        return;
    int x = static_cast<int>(pixelX / m_fieldWidth);
    int y = static_cast<int>(pixelY / m_fieldHeight);
    Piece piece = m_piece[x][y];
    if (piece.is_null() || m_disabled[x][y])
        return;
    update();
    emit pieceSelected(m_color, piece, m_transform[x][y]);
}

void PieceSelector::paintEvent(QPaintEvent*)
{
    setDisabledStatus(m_disabled);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    auto pieceSet = m_bd.get_piece_set();
    bool isTrigon = (pieceSet == PieceSet::trigon);
    bool isNexos = (pieceSet == PieceSet::nexos);
    bool isCallisto = (pieceSet == PieceSet::callisto);
    qreal ratio;
    if (isTrigon)
    {
        ratio = 1.732;
        m_fieldWidth = min(qreal(width()) / (m_nuColumns + 1),
                           qreal(height()) / (ratio * m_nuRows));
    }
    else
    {
        ratio = 1;
        m_fieldWidth = min(qreal(width()) / m_nuColumns,
                           qreal(height()) / m_nuRows);
    }
    if (m_fieldWidth > 8)
        // Prefer pixel alignment if piece is not too small
        m_fieldWidth = floor(m_fieldWidth);
    m_fieldHeight = ratio * m_fieldWidth;
    m_selectorWidth = m_fieldWidth * m_nuColumns;
    m_selectorHeight = m_fieldHeight * m_nuRows;
    painter.save();
    painter.translate(0.5 * (width() - m_selectorWidth),
                      0.5 * (height() - m_selectorHeight));
    auto variant = m_bd.get_variant();
    auto& geo = m_bd.get_geometry();
    for (unsigned x = 0; x < m_nuColumns; ++x)
        for (unsigned y = 0; y < m_nuRows; ++y)
        {
            auto pointType = geo.get_point_type(x, y);
            Piece piece = m_piece[x][y];
            if (isTrigon)
            {
                if (piece.is_null() || m_disabled[x][y])
                    continue;
                bool isUpward = (pointType == geo.get_point_type(0, 0));
                Util::paintColorTriangle(painter, variant, m_color, isUpward,
                                         x * m_fieldWidth, y * m_fieldHeight,
                                         m_fieldWidth, m_fieldHeight);
            }
            else if (isNexos)
            {
                if (pointType == 1 || pointType == 2)
                {
                    if (piece.is_null() || m_disabled[x][y])
                        continue;
                    bool isHorizontal = (geo.get_point_type(x, y) == 1);
                    Util::paintColorSegment(painter, variant, m_color,
                                            isHorizontal, x * m_fieldWidth,
                                            y * m_fieldHeight, m_fieldWidth);
                }
                else if (pointType == 0)
                {
                    bool hasLeft =
                            (x > 0 && ! m_piece[x - 1][y].is_null()
                             && ! m_disabled[x - 1][y]);
                    bool hasRight =
                            (x < m_nuColumns - 1
                             && ! m_piece[x + 1][y].is_null()
                             && ! m_disabled[x + 1][y]);
                    bool hasUp =
                            (y > 0 && ! m_piece[x][y - 1].is_null()
                             && ! m_disabled[x][y - 1]);
                    bool hasDown =
                            (y < m_nuRows - 1
                             && ! m_piece[x][y + 1].is_null()
                             && ! m_disabled[x][y + 1]);
                    Util::paintJunction(painter, variant, m_color,
                                        x * m_fieldWidth, y * m_fieldHeight,
                                        m_fieldWidth, m_fieldHeight, hasLeft,
                                        hasRight, hasUp, hasDown);
                }
            }
            else
            {
                if (piece.is_null() || m_disabled[x][y])
                    continue;
                if (isCallisto)
                {
                    bool hasLeft = (x > 0 && ! m_piece[x - 1][y].is_null());
                    bool hasRight =
                            (x < m_nuColumns - 1
                             && ! m_piece[x + 1][y].is_null());
                    bool hasUp = (y > 0 && ! m_piece[x][y - 1].is_null());
                    bool hasDown =
                            (y < m_nuRows - 1
                             && ! m_piece[x][y + 1].is_null());
                    bool isOnePiece =
                            (! hasLeft && ! hasRight && ! hasUp && ! hasDown);
                    Util::paintColorSquareCallisto(painter, variant, m_color,
                                                   x * m_fieldWidth,
                                                   y * m_fieldHeight,
                                                   m_fieldWidth, hasRight,
                                                   hasDown, isOnePiece);
                }
                else
                    Util::paintColorSquare(painter, variant, m_color,
                                           x * m_fieldWidth, y * m_fieldHeight,
                                           m_fieldWidth);
            }
        }
    painter.restore();
}

void PieceSelector::setDisabledStatus(bool disabledStatus[maxColumns][maxRows])
{
    bool marker[maxColumns][maxRows];
    for (unsigned x = 0; x < m_nuColumns; ++x)
        for (unsigned y = 0; y < m_nuRows; ++y)
        {
            marker[x][y] = false;
            disabledStatus[x][y] = false;
        }
    PieceMap<unsigned> nuInstances;
    nuInstances.fill(0);
    bool isColorUsed = (m_color.to_int() < m_bd.get_nu_colors());
    for (unsigned x = 0; x < m_nuColumns; ++x)
        for (unsigned y = 0; y < m_nuRows; ++y)
        {
            if (marker[x][y])
                continue;
            Piece piece = m_piece[x][y];
            if (piece.is_null())
                continue;
            PiecePoints points;
            findPiecePoints(piece, x, y, points);
            bool disabled = false;
            if (! isColorUsed
                    || ++nuInstances[piece] > m_bd.get_nu_left_piece(m_color,
                                                                     piece))
                disabled = true;
            for (auto& p : points)
            {
                disabledStatus[p.x][p.y] = disabled;
                marker[p.x][p.y] = true;
            }
        }
}

//-----------------------------------------------------------------------------
