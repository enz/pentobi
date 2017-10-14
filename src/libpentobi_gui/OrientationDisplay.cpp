//-----------------------------------------------------------------------------
/** @file libpentobi_gui/OrientationDisplay.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "OrientationDisplay.h"

#include <QPainter>
#include "libboardgame_base/GeometryUtil.h"
#include "libpentobi_gui/Util.h"

using namespace std;
using libboardgame_base::ArrayList;
using libboardgame_base::CoordPoint;
using libboardgame_base::Transform;
using libboardgame_base::geometry_util::normalize_offset;
using libboardgame_base::geometry_util::type_match_offset;
using libboardgame_base::geometry_util::type_match_shift;
using libpentobi_base::Geometry;
using libpentobi_base::PiecePoints;
using libpentobi_base::PieceSet;

//-----------------------------------------------------------------------------

OrientationDisplay::OrientationDisplay(QWidget* parent, const Board& bd)
    : QWidget(parent),
      m_bd(bd)
{
    setMinimumSize(30, 30);
}

void OrientationDisplay::clearSelectedColor()
{
    if (m_isColorSelected)
    {
        m_isColorSelected = false;
        update();
    }
}

void OrientationDisplay::clearPiece()
{
    if (m_piece.is_null())
        return;
    m_piece = Piece::null();
    update();
}

void OrientationDisplay::mousePressEvent(QMouseEvent* event)
{
    LIBBOARDGAME_UNUSED(event);
    if (m_isColorSelected && m_piece.is_null())
        emit colorClicked(m_color);
}

void OrientationDisplay::paintEvent(QPaintEvent* event)
{
    LIBBOARDGAME_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    auto variant = m_bd.get_variant();
    qreal fieldWidth;
    qreal fieldHeight;
    qreal displayWidth;
    qreal displayHeight;
    auto pieceSet = m_bd.get_piece_set();
    bool isTrigon = (pieceSet == PieceSet::trigon);
    bool isNexos = (pieceSet == PieceSet::nexos);
    bool isCallisto = (pieceSet == PieceSet::callisto);
    bool isGembloQ = (pieceSet == PieceSet::gembloq);
    qreal ratio;
    int columns;
    int rows;
    if (isTrigon)
    {
        ratio = 1.732;
        columns = 7;
        rows = 4;
    }
    else if (isGembloQ)
    {
        ratio = 2;
        columns = 12;
        rows = 6;
    }
    else if (isNexos)
    {
        ratio = 1;
        columns = 8;
        rows = 8;
    }
    else
    {
        ratio = 1;
        columns = 5;
        rows = 5;
    }
    fieldWidth = min(qreal(width()) / columns,
                     qreal(height()) / (ratio * rows));
    if (fieldWidth > 8)
        // Prefer pixel alignment if piece is not too small
        fieldWidth = floor(fieldWidth);
    fieldHeight = ratio * fieldWidth;
    displayWidth = fieldWidth * columns;
    displayHeight = fieldHeight * rows;
    if (m_piece.is_null())
    {
        if (m_isColorSelected)
        {
            qreal dotSize = 0.07 * height();
            QColor color = Util::getPaintColor(variant, m_color);
            painter.setBrush(color);
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(QPointF(0.5 * width(), 0.5 * height()),
                                dotSize, dotSize);
        }
        return;
    }
    painter.save();
    painter.translate(0.5 * (width() - displayWidth),
                      0.5 * (height() - displayHeight));
    PiecePoints points = m_bd.get_piece_info(m_piece).get_points();
    m_transform->transform(points.begin(), points.end());
    auto& geo = m_bd.get_geometry();
    type_match_shift(geo, points.begin(), points.end(),
                     m_transform->get_point_type());
    unsigned width;
    unsigned height;
    CoordPoint offset;
    normalize_offset(points.begin(), points.end(), width, height, offset);
    offset = type_match_offset(geo, geo.get_point_type(offset));
    painter.save();
    painter.translate(0.5 * (displayWidth - width * fieldWidth),
                      0.5 * (displayHeight - height * fieldHeight));
    ArrayList<CoordPoint, 2 * PieceInfo::max_size> junctions;
    for (CoordPoint p : points)
    {
        qreal x = p.x * fieldWidth;
        qreal y = p.y * fieldHeight;
        auto pointType = geo.get_point_type(p + offset);
        if (isTrigon)
        {
            bool isUpward = (pointType == 0);
            Util::paintColorTriangle(painter, variant, m_color, isUpward,
                                     x, y, fieldWidth, fieldHeight);
        }
        else if (isGembloQ)
            Util::paintColorGembloQ(painter, variant, m_color, pointType, x, y,
                                    fieldWidth);
        else if (isNexos)
        {
            if (pointType == 1 || pointType == 2)
            {
                bool isHorizontal = (pointType == 1);
                Util::paintColorSegment(painter, variant, m_color,
                                        isHorizontal, x, y, fieldWidth);
                if (pointType == 1) // Horiz. segment
                {
                    junctions.include(CoordPoint(p.x - 1, p.y));
                    junctions.include(CoordPoint(p.x + 1, p.y));
                }
                else
                {
                    LIBBOARDGAME_ASSERT(pointType == 2); // Vert. segment
                    junctions.include(CoordPoint(p.x, p.y - 1));
                    junctions.include(CoordPoint(p.x, p.y + 1));
                }
            }
        }
        else if (isCallisto)
        {
            bool hasRight = points.contains(CoordPoint(p.x + 1, p.y));
            bool hasDown = points.contains(CoordPoint(p.x, p.y + 1));
            bool isOnePiece = (points.size() == 1);
            Util::paintColorSquareCallisto(painter, variant, m_color, x, y,
                                           fieldWidth, hasRight, hasDown,
                                           isOnePiece);
        }
        else
            Util::paintColorSquare(painter, variant, m_color, x, y,
                                   fieldWidth);
    }
    if (isNexos)
        for (CoordPoint p : junctions)
        {
            bool hasLeft = points.contains(CoordPoint(p.x - 1, p.y));
            bool hasRight = points.contains(CoordPoint(p.x + 1, p.y));
            bool hasUp = points.contains(CoordPoint(p.x, p.y - 1));
            bool hasDown = points.contains(CoordPoint(p.x, p.y + 1));
            Util::paintJunction(painter, variant, m_color, p.x * fieldWidth,
                                p.y * fieldHeight, fieldWidth, fieldHeight,
                                hasLeft, hasRight, hasUp, hasDown);
        }
    painter.restore();
    painter.restore();
}

void OrientationDisplay::selectColor(Color c)
{
    if (m_isColorSelected && m_color == c)
        return;
    m_isColorSelected = true;
    m_color = c;
    update();
}

void OrientationDisplay::setSelectedPiece(Piece piece)
{
    auto transform = m_bd.get_transforms().get_default();
    if (m_piece == piece && m_transform == transform)
        return;
    m_piece = piece;
    m_transform = transform;
    update();
}

void OrientationDisplay::setSelectedPieceTransform(const Transform* transform)
{
    if (m_transform == transform)
        return;
    m_transform = transform;
    update();
}

//-----------------------------------------------------------------------------
