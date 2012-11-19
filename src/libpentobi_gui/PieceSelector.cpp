//-----------------------------------------------------------------------------
/** @file libpentobi_gui/PieceSelector.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PieceSelector.h"

#include <algorithm>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <QMouseEvent>
#include <QPainter>
#include "libboardgame_base/GeometryUtil.h"
#include "libpentobi_gui/Util.h"

using namespace std;
using boost::trim;
using libboardgame_base::CoordPoint;
using libboardgame_base::geometry_util::type_match_shift;
using libboardgame_util::log;
using libpentobi_base::board_type_trigon;
using libpentobi_base::board_type_trigon_3;
using libpentobi_base::variant_junior;
using libpentobi_base::BoardConst;
using libpentobi_base::BoardType;
using libpentobi_base::Variant;
using libpentobi_base::Geometry;
using libpentobi_base::PieceMap;

//-----------------------------------------------------------------------------

namespace {

const string pieceLayoutClassic =
    " 1 .V3V3 .L4L4L4 .T4 . . .Z4Z4 .L5L5L5L5 .V5V5V5 . . . . N N . . . ."
    " . . .V3 . . .L4 .T4T4 .Z4Z4 . .L5 . . . .V5 . . .T5 . . . N N N .I5"
    " 2 2 . . .I4 . . .T4 . . . . . . . . X . .V5 .T5T5T5 . . . . . . .I5"
    " . . .I3 .I4 . P . . . W W . F . . X X X . . . . .T5 .Z5Z5 . U U .I5"
    " O O .I3 .I4 . P P . W W . . F F . . X . . Y . . . . .Z5 . . . U .I5"
    " O O .I3 .I4 . P P . W . . F F . . . . . Y Y Y Y . .Z5Z5 . . U U .I5";

const string pieceLayoutJunior =
    "1 . 1 . V3V3. . L4L4L4. T4T4T4. . O O . O O . P P . . I5. I5. . L5L5"
    ". . . . V3. . . L4. . . . T4. . . O O . O O . P P . . I5. I5. . . L5"
    "2 . 2 . . . V3. . . . L4. . . T4. . . . . . . P . . . I5. I5. L5. L5"
    "2 . 2 . . V3V3. . L4L4L4. . T4T4T4. . Z4. Z4. . . P . I5. I5. L5. L5"
    ". . . . . . . . . . . . . . . . . . Z4Z4. Z4Z4. P P . I5. I5. L5. . "
    "I3I3I3. I3I3I3. I4I4I4I4. I4I4I4I4. Z4. . . Z4. P P . . . . . L5L5. ";

const string pieceLayoutTrigon =
    "L5L5 . . F F F F . .L6L6 . . O O O . . X X X . . .A6A6 . . G G G G . . Y Y . . . .I4I4I4I4"
    "L5L5 . . F . F . . .L6L6 . . O O O . . X X X . .A6A6A6A6 . . . G G . . Y Y Y Y . . . . . ."
    " .L5 . . . . . . S . .L6L6 . . . . . . . . . . . . . . . . . . . . . . . . . . . . .I3I3I3"
    " . . . . . S S S S . . . . . . . .P5P5P5P5 . . . . W . . . .C4C4 . .I5I5I5I5I5 . . . . . ."
    "C5 .C5 . . S . . . . V V . .P6 . . . .P5 . .A4 . . W W W . .C4C4 . . . . . . . . . . . 2 2"
    "C5C5C5 . . . . V V V V . .P6P6P6P6P6 . . .A4A4A4 . . W W . . . . .I6I6I6I6I6I6 . . 1 . . .";
}

//-----------------------------------------------------------------------------

PieceSelector::PieceSelector(QWidget* parent, const Board& bd, Color color)
    : QWidget(parent),
      m_bd(bd),
      m_color(color)
{
    setMinimumWidth(170);
    setMinimumHeight(30);
    init();
}

void PieceSelector::checkUpdate()
{
    bool disabledStatus[maxColumns][maxRows];
    setDisabledStatus(disabledStatus);
    bool changed = false;
    for (unsigned x = 0; x < m_nuColumns; ++x)
        for (unsigned y = 0; y < m_nuRows; ++y)
            if (! m_piece[x][y].is_null()
                && disabledStatus[x][y] != m_disabledStatus[x][y])
            {
                changed = true;
                break;
            }
    if (changed)
        update();
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
    // we would need to iterate over neighboring CoordPoint's like AdjIterator
    // iterates over neighboring Point's
    findPiecePoints(piece, x + 1, y, points);
    findPiecePoints(piece, x - 1, y, points);
    findPiecePoints(piece, x, y + 1, points);
    findPiecePoints(piece, x, y - 1, points);
}

int PieceSelector::heightForWidth(int width) const
{
    // Use ratio for layout of classic pieces, which has larger relative width
    // because the limiting factor in the right panel of the main window is the
    // width
    return width / 34 * 6;
}

void PieceSelector::init()
{
    BoardType boardType = m_bd.get_board_type();
    Variant variant = m_bd.get_variant();
    const string* pieceLayout;
    if (boardType == board_type_trigon || boardType == board_type_trigon_3)
    {
        pieceLayout = &pieceLayoutTrigon;
        m_nuColumns = 45;
        m_nuRows = 6;
    }
    else if (variant == variant_junior)
    {
        pieceLayout = &pieceLayoutJunior;
        m_nuColumns = 34;
        m_nuRows = 6;
    }
    else
    {
        pieceLayout = &pieceLayoutClassic;
        m_nuColumns = 34;
        m_nuRows = 6;
    }
    for (unsigned y = 0; y < m_nuRows; ++y)
        for (unsigned x = 0; x < m_nuColumns; ++x)
        {
            string name = pieceLayout->substr(y * m_nuColumns * 2 + x * 2, 2);
            trim(name);
            Piece piece = Piece::null();
            if (name != ".")
            {
                m_bd.get_piece_by_name(name, piece);
                LIBBOARDGAME_ASSERT(! piece.is_null());
            }
            m_piece[x][y] = piece;
        }
    const Geometry& geometry = m_bd.get_geometry();
    for (unsigned y = 0; y < m_nuRows; ++y)
        for (unsigned x = 0; x < m_nuColumns; ++x)
        {
            Piece piece = m_piece[x][y];
            if (piece.is_null())
                continue;
            PiecePoints points;
            findPiecePoints(piece, x, y, points);
            // Mirror y to match the convention of CoordPoint coordinates
            BOOST_FOREACH(CoordPoint& p, points)
                p.y = m_nuRows - p.y - 1;
            type_match_shift(geometry, points.begin(), points.end(), 0);
            m_transform[x][y] =
                m_bd.get_piece_info(piece).find_transform(geometry, points);
            LIBBOARDGAME_ASSERT(m_transform[x][y] != 0);
        }
    setDisabledStatus(m_disabledStatus);
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
    if (piece.is_null() || m_disabledStatus[x][y])
        return;
    update();
    emit pieceSelected(m_color, piece, m_transform[x][y]);
}

void PieceSelector::paintEvent(QPaintEvent*)
{
    setDisabledStatus(m_disabledStatus);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    BoardType boardType = m_bd.get_board_type();
    bool isTrigon =
        (boardType == board_type_trigon || boardType == board_type_trigon_3);
    if (isTrigon)
    {
        qreal ratio = 1.732;
        m_fieldWidth = min(qreal(width()) / (m_nuColumns + 1),
                           qreal(height()) / (ratio * m_nuRows));
        m_fieldHeight = ratio * m_fieldWidth;
    }
    else
    {
        m_fieldWidth = min(qreal(width()) / m_nuColumns,
                           qreal(height()) / m_nuRows);
        m_fieldHeight = m_fieldWidth;
    }
    m_selectorWidth = m_fieldWidth * m_nuColumns;
    m_selectorHeight = m_fieldHeight * m_nuRows;
    painter.save();
    painter.translate(0.5 * (width() - m_selectorWidth),
                      0.5 * (height() - m_selectorHeight));
    Variant variant = m_bd.get_variant();
    const Geometry& geometry = m_bd.get_geometry();
    for (unsigned x = 0; x < m_nuColumns; ++x)
        for (unsigned y = 0; y < m_nuRows; ++y)
        {
            Piece piece = m_piece[x][y];
            if (! piece.is_null() && ! m_disabledStatus[x][y])
            {
                if (isTrigon)
                {
                    bool isUpside =
                        (geometry.get_point_type(x, m_nuRows - y - 1)
                         != geometry.get_point_type(0, 0));
                    Util::paintColorTriangle(painter, variant, m_color,
                                             isUpside, x * m_fieldWidth,
                                             y * m_fieldHeight, m_fieldWidth,
                                             m_fieldHeight);
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
            BOOST_FOREACH(CoordPoint p, points)
            {
                disabledStatus[p.x][p.y] = disabled;
                marker[p.x][p.y] = true;
            }
        }
}

//-----------------------------------------------------------------------------
