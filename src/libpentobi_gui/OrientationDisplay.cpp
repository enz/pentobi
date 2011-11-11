//-----------------------------------------------------------------------------
/** @file libpentobi_gui/OrientationDisplay.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "OrientationDisplay.h"

#include "libpentobi_gui/Util.h"

using namespace std;
using libboardgame_base::CoordPoint;
using libboardgame_base::Transform;
using libpentobi_base::Piece;

//-----------------------------------------------------------------------------

OrientationDisplay::OrientationDisplay(QWidget* parent, const Board& bd)
    : QWidget(parent),
      m_bd(bd),
      m_piece(0),
      m_transform(0),
      m_isColorSelected(false)
{
    setMinimumWidth(5 * Piece::max_size);
    setMinimumHeight(5 * Piece::max_size);
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
    if (m_piece == 0)
    {
        if (m_isColorSelected)
        {
            QColor color =
                Util::getPaintColor(m_bd.get_game_variant(), m_color);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setPen(color);
            painter.setBrush(color);
            painter.drawEllipse(width() / 2 - 5, height() / 2 - 5, 10, 10);
        }
        return;
    }
    int squareSize = min(width() / Piece::max_size, height() / Piece::max_size);
    int displaySize = squareSize * Piece::max_size;
    painter.save();
    painter.translate((width() - displaySize) / 2,
                      (height() - displaySize) / 2);
    Piece::Points points = m_piece->get_points();
    m_transform->transform(points.begin(), points.end());
    unsigned int width, height;
    CoordPoint::normalize_offset(points.begin(), points.end(), width, height);
    painter.save();
    painter.translate((displaySize - width * squareSize) / 2,
                      (displaySize - height * squareSize) / 2);
    BOOST_FOREACH(CoordPoint p, points)
    {
        int x = p.x * squareSize;
        int y = (height - p.y - 1) * squareSize;
        Util::paintColorSquare(painter, m_bd.get_game_variant(), m_color, x, y,
                               squareSize);
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
