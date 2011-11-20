//-----------------------------------------------------------------------------
/** @file libpentobi_gui/OrientationDisplay.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "OrientationDisplay.h"

#include "libboardgame_base/GeometryUtil.h"
#include "libpentobi_gui/Util.h"

using namespace std;
using libboardgame_base::CoordPoint;
using libboardgame_base::Transform;
using libboardgame_base::geometry_util::normalize_offset;
using libboardgame_base::geometry_util::type_preserve_shift;
using libpentobi_base::board_type_trigon;
using libpentobi_base::BoardType;
using libpentobi_base::Geometry;
using libpentobi_base::Piece;

//-----------------------------------------------------------------------------

OrientationDisplay::OrientationDisplay(QWidget* parent, const Board& bd)
    : QWidget(parent),
      m_bd(bd),
      m_piece(0),
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
    if (m_piece == 0)
        return;
    m_piece = 0;
    update();
}

void OrientationDisplay::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setClipRegion(event->region());
    painter.setRenderHint(QPainter::Antialiasing, true);
    if (m_piece == 0)
    {
        if (m_isColorSelected)
        {
            QColor color =
                Util::getPaintColor(m_bd.get_game_variant(), m_color);
            painter.setPen(color);
            painter.setBrush(color);
            painter.drawEllipse(width() / 2 - 5, height() / 2 - 5, 10, 10);
        }
        return;
    }
    qreal fieldSize = min(width() / 7.f, height() / 7.f);
    qreal displaySize = fieldSize * 7;
    painter.save();
    painter.translate(0.5 * (width() - displaySize),
                      0.5 * (height() - displaySize));
    Piece::Points points = m_piece->get_points();
    m_transform->transform(points.begin(), points.end());
    const Geometry& geometry = m_bd.get_geometry();
    type_preserve_shift(geometry, points.begin(), points.end(),
                        m_transform->get_new_point_type());
    unsigned int width;
    unsigned int height;
    CoordPoint offset;
    normalize_offset(geometry, points.begin(), points.end(), width, height,
                     offset);
    bool invertPointType = (geometry.get_point_type(offset) != 0);
    painter.save();
    painter.translate(0.5 * (displaySize - width * fieldSize),
                      0.5 * (displaySize - height * fieldSize));
    GameVariant game_variant = m_bd.get_game_variant();
    BoardType board_type = m_bd.get_board_type();
    BOOST_FOREACH(CoordPoint p, points)
    {
        qreal x = p.x * fieldSize;
        qreal y = (height - p.y - 1) * fieldSize;
        if (board_type == board_type_trigon)
        {
            bool isUpside = (geometry.get_point_type(p) != 0);
            if (invertPointType)
                isUpside = ! isUpside;
            Util::paintColorTriangle(painter, game_variant, m_color, isUpside,
                                     x, y, fieldSize, fieldSize);
        }
        else
            Util::paintColorSquare(painter, game_variant, m_color, x, y,
                                   fieldSize);
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

void OrientationDisplay::setSelectedPiece(const Piece& piece)
{
    const Transform* transform = m_bd.get_transforms().get_default();
    if (m_piece == &piece && m_transform == transform)
        return;
    m_piece = &piece;
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
