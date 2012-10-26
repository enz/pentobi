//-----------------------------------------------------------------------------
/** @file libpentobi_gui/OrientationDisplay.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "OrientationDisplay.h"

#include <boost/foreach.hpp>
#include <QPainter>
#include "libboardgame_base/GeometryUtil.h"
#include "libpentobi_gui/Util.h"

using namespace std;
using libboardgame_base::CoordPoint;
using libboardgame_base::Transform;
using libboardgame_base::geometry_util::normalize_offset;
using libboardgame_base::geometry_util::type_match_shift;
using libpentobi_base::board_type_trigon;
using libpentobi_base::board_type_trigon_3;
using libpentobi_base::BoardType;
using libpentobi_base::Geometry;
using libpentobi_base::PiecePoints;

//-----------------------------------------------------------------------------

OrientationDisplay::OrientationDisplay(QWidget* parent, const Board& bd)
    : QWidget(parent),
      m_bd(bd),
      m_piece(Piece::null()),
      m_transform(0),
      m_isColorSelected(false)
{
    setMinimumWidth(30);
    setMinimumHeight(30);
}

void OrientationDisplay::clearSelectedColor()
{
    if (m_isColorSelected)
    {
        m_isColorSelected = false;
        update();
    }
}

void OrientationDisplay::clearSelectedPiece()
{
    if (m_piece.is_null())
        return;
    m_piece = Piece::null();
    update();
}

int OrientationDisplay::heightForWidth(int width) const
{
    return width;
}

void OrientationDisplay::mousePressEvent(QMouseEvent*)
{
    if (m_isColorSelected && m_piece.is_null())
        emit colorClicked(m_color);
}

void OrientationDisplay::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    BoardType board_type = m_bd.get_board_type();
    qreal fieldWidth;
    qreal fieldHeight;
    qreal displayWidth;
    qreal displayHeight;
    bool isTrigon =
        (board_type == board_type_trigon || board_type == board_type_trigon_3);
    if (isTrigon)
    {
        int columns = 7;
        int rows = 5;
        qreal ratio = 1.732;
        fieldWidth = min(qreal(width()) / columns,
                               qreal(height()) / (ratio * rows));
        fieldHeight = ratio * fieldWidth;
        displayWidth = fieldWidth * columns;
        displayHeight = fieldHeight * rows;
    }
    else
    {
        int columns = 6;
        int rows = 6;
        fieldWidth = min(qreal(width()) / columns,
                               qreal(height()) / rows);
        fieldHeight = fieldWidth;
        displayWidth = fieldWidth * columns;
        displayHeight = fieldHeight * rows;
    }
    if (m_piece.is_null())
    {
        if (m_isColorSelected)
        {
            qreal dotSize = 0.07 * height();
            QColor color = Util::getPaintColor(m_bd.get_variant(), m_color);
            painter.setPen(Qt::NoPen);
            painter.setBrush(color);
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
    const Geometry& geometry = m_bd.get_geometry();
    type_match_shift(geometry, points.begin(), points.end(),
                     m_transform->get_new_point_type());
    unsigned width;
    unsigned height;
    CoordPoint offset;
    normalize_offset(points.begin(), points.end(), width, height, offset);
    bool invertPointType =
        (geometry.get_point_type(offset) != geometry.get_point_type(0, 0));
    painter.save();
    painter.translate(0.5 * (displayWidth - width * fieldWidth),
                      0.5 * (displayHeight - height * fieldHeight));
    Variant variant = m_bd.get_variant();
    BOOST_FOREACH(CoordPoint p, points)
    {
        qreal x = p.x * fieldWidth;
        qreal y = (height - p.y - 1) * fieldHeight;
        if (isTrigon)
        {
            bool isUpside = (geometry.get_point_type(p) != 0);
            if (invertPointType)
                isUpside = ! isUpside;
            Util::paintColorTriangle(painter, variant, m_color, isUpside,
                                     x, y, fieldWidth, fieldHeight);
        }
        else
            Util::paintColorSquare(painter, variant, m_color, x, y, fieldWidth);
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
    const Transform* transform = m_bd.get_transforms().get_default();
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
