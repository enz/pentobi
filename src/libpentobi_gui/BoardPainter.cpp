//-----------------------------------------------------------------------------
/** @file libpentobi_gui/BoardPainter.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardPainter.h"

#include <algorithm>
#include <cmath>
#include "Util.h"
#include "libboardgame_util/Log.h"
#include "libpentobi_base/AdjIterator.h"
#include "libpentobi_base/DiagIterator.h"

using namespace std;
using libboardgame_util::log;
using libpentobi_base::game_variant_trigon;
using libpentobi_base::game_variant_trigon_2;
using libpentobi_base::game_variant_trigon_3;
using libpentobi_base::AdjIterator;
using libpentobi_base::BoardIterator;
using libpentobi_base::DiagIterator;
using libpentobi_base::GeometryIterator;
using libpentobi_base::Move;
using libpentobi_base::PointState;

//-----------------------------------------------------------------------------

BoardPainter::BoardPainter()
    : m_hasPainted(false),
      m_drawCoordLabels(false),
      m_coordLabelColor(Qt::black)
{
}

void BoardPainter::drawCoordinates(QPainter& painter, bool isTrigon)
{
    painter.setPen(m_coordLabelColor);
    for (int x = 0; x < m_width; ++x)
    {
        QString label;
        if (x < 26)
            label = QString(QChar('A' + x));
        else
        {
            label = "A";
            label.append(QChar('A' + (x - 26)));
        }
        drawLabel(painter, x * m_fieldWidth, m_height * m_fieldHeight,
                  m_fieldWidth, m_fieldHeight, label, false, true);
        drawLabel(painter, x * m_fieldWidth, -m_fieldHeight,
                  m_fieldWidth, m_fieldHeight, label, false, true);
    }
    for (int y = 0; y < m_height; ++y)
    {
        QString label;
        label.setNum(y + 1);
        qreal left;
        qreal right;
        if (isTrigon)
        {
            left = -1.5 * m_fieldWidth;
            right = (m_width + 0.5) * m_fieldWidth;
        }
        else
        {
            left = -m_fieldWidth;
            right = m_width * m_fieldWidth;
        }
        drawLabel(painter, left, (m_height - y - 1) * m_fieldHeight,
                  m_fieldWidth, m_fieldHeight, label, false, true);
        drawLabel(painter, right, (m_height - y - 1) * m_fieldHeight,
                  m_fieldWidth, m_fieldHeight, label, false, true);
    }
}

void BoardPainter::drawLabel(QPainter& painter, qreal x, qreal y,
                             qreal width, qreal height, const QString& label,
                             bool underline, bool small)
{
    if (small)
        painter.setFont(m_fontSmall);
    else if (underline)
        painter.setFont(m_fontUnderlined);
    else
        painter.setFont(m_font);
    QFontMetrics metrics(painter.font());
    QRect boundingRect = metrics.boundingRect(label);
    qreal dx = 0.5 * (width - boundingRect.width());
    qreal dy = 0.5 * (height - boundingRect.height());
    QRectF rect;
    rect.setCoords(floor(x + dx), floor(y + dy),
                   ceil(x + width - dx + 1), ceil(y + height - dy + 1));
    painter.drawText(rect, Qt::TextDontClip, label);
}

void BoardPainter::drawLabels(QPainter& painter,
                              const FullGrid<PointStateExt>& pointState,
                              GameVariant gameVariant,
                              const Grid<QString>* labels,
                              const Grid<MarkupFlags>* markupFlags)
{
    if (labels == 0)
        return;
    const Geometry& geometry = pointState.get_geometry();
    bool isTrigon = (gameVariant == game_variant_trigon
                     || gameVariant == game_variant_trigon_2
                     || gameVariant == game_variant_trigon_3);
    for (GeometryIterator i(geometry); i; ++i)
        if (! (*labels)[*i].isEmpty())
        {
            PointState s = pointState[*i].to_point_state();
            painter.setPen(Util::getLabelColor(gameVariant, s));
            bool underline = false;
            if (markupFlags != 0 && (*markupFlags)[*i].test(markup_variation))
                underline = true;
            qreal x = i->get_x() * m_fieldWidth;
            qreal y = (m_height - i->get_y() - 1) * m_fieldHeight;
            qreal width = m_fieldWidth;
            qreal height = m_fieldHeight;
            if (isTrigon)
            {
                bool isUpside = (geometry.get_point_type(*i) == 1);
                if (isUpside)
                    y += 0.333 * height;
                height = 0.666 * height;
            }
            drawLabel(painter, x, y, width, height, (*labels)[*i], underline,
                      false);
        }
}

CoordPoint BoardPainter::getCoordPoint(int x, int y)
{
    if (! m_hasPainted)
        return CoordPoint::null();
    x = (x - m_boardOffset.x()) / m_fieldWidth;
    y = (y - m_boardOffset.y()) / m_fieldHeight;
    y = m_height - y - 1;
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return CoordPoint::null();
    else
        return CoordPoint(x, y);
}

void BoardPainter::paintEmptyBoard(QPainter& painter, unsigned int width,
                                   unsigned int height, GameVariant gameVariant,
                                   const Geometry& geometry)
{
    m_hasPainted = true;
    painter.setRenderHint(QPainter::Antialiasing, true);
    m_gameVariant = gameVariant;
    m_geometry = &geometry;
    m_width = static_cast<int>(m_geometry->get_width());
    m_height = static_cast<int>(m_geometry->get_height());
    m_isTrigon = (gameVariant == game_variant_trigon
                  || gameVariant == game_variant_trigon_2
                  || gameVariant == game_variant_trigon_3);
    if (m_isTrigon)
    {
        qreal ratio = 1.732;
        if (m_drawCoordLabels)
            m_fieldWidth =
                min(qreal(width) / (m_width + 3),
                    height / (ratio * (m_height + 2)));
        else
            m_fieldWidth =
                min(qreal(width) / (m_width + 1), height / (ratio * m_height));
        m_fieldHeight = ratio * m_fieldWidth;
        m_boardOffset =
            QPointF(0.5 * (width - m_fieldWidth * m_width),
                    0.5 * (height - m_fieldHeight * m_height));
    }
    else
    {
        if (m_drawCoordLabels)
            m_fieldWidth =
                min(qreal(width) / (m_width + 2),
                    qreal(height) / (m_height + 2));
        else
            m_fieldWidth =
                min(qreal(width) / m_width, qreal(height) / m_height);
        m_fieldHeight = m_fieldWidth;
        m_boardOffset =
            QPointF(0.5 * (width - m_fieldWidth * m_width),
                    0.5 * (height - m_fieldHeight * m_height));
    }
    m_fontSmall = m_font;
    if (m_isTrigon)
    {
        m_font.setPointSizeF(0.55 * m_fieldWidth);
        m_fontSmall.setPointSizeF(0.4 * m_fieldWidth);
    }
    else
    {
        m_font.setPointSizeF(0.4 * m_fieldWidth);
        m_fontSmall.setPointSizeF(0.34 * m_fieldWidth);
    }
    m_fontUnderlined = m_font;
    m_fontUnderlined.setUnderline(true);
    painter.save();
    painter.translate(m_boardOffset);
    if (m_drawCoordLabels)
        drawCoordinates(painter, m_isTrigon);
    m_startingPoints.init(gameVariant, *m_geometry);
    for (GeometryIterator i(*m_geometry); i; ++i)
    {
        int x = i->get_x();
        int y = i->get_y();
        qreal fieldX = x * m_fieldWidth;
        qreal fieldY = (m_height - y - 1) * m_fieldHeight;
        if (m_isTrigon)
        {
            bool isUpside = (m_geometry->get_point_type(x, y) == 1);
            if (m_startingPoints.is_colorless_starting_point(*i))
                Util::paintEmptyTriangleStartingPoint(painter, isUpside, fieldX,
                                                      fieldY, m_fieldWidth,
                                                      m_fieldHeight);
            else
                Util::paintEmptyTriangle(painter, isUpside, fieldX, fieldY,
                                         m_fieldWidth, m_fieldHeight);
        }
        else
        {
            if (m_startingPoints.is_colored_starting_point(*i))
            {
                Color color = m_startingPoints.get_starting_point_color(*i);
                Util::paintEmptySquareStartingPoint(painter, gameVariant,
                                                    color, fieldX, fieldY,
                                                    m_fieldWidth);
            }
            else
                Util::paintEmptySquare(painter, fieldX, fieldY, m_fieldWidth);
        }
    }
    painter.restore();
}

void BoardPainter::paintPieces(QPainter& painter,
                               const FullGrid<PointStateExt>& pointState,
                               const Grid<QString>* labels,
                               const Grid<MarkupFlags>* markupFlags)
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.save();
    painter.translate(m_boardOffset);
    for (GeometryIterator i(*m_geometry); i; ++i)
    {
        int x = i->get_x();
        int y = i->get_y();
        PointStateExt s = pointState[*i];
        qreal fieldX = x * m_fieldWidth;
        qreal fieldY = (m_height - y - 1) * m_fieldHeight;
        if (m_isTrigon)
        {
            bool isUpside = (m_geometry->get_point_type(x, y) == 1);
            if (s.is_color())
                Util::paintColorTriangle(painter, m_gameVariant, s.to_color(),
                                         isUpside, fieldX, fieldY,
                                         m_fieldWidth, m_fieldHeight);
        }
        else
        {
            if (s.is_color())
                Util::paintColorSquare(painter, m_gameVariant, s.to_color(),
                                       fieldX, fieldY, m_fieldWidth);
        }
    }
    drawLabels(painter, pointState, m_gameVariant, labels, markupFlags);
    painter.restore();
}

void BoardPainter::paintSelectedPiece(QPainter& painter, Color c,
                                      const MovePoints& points, bool isLegal)
{
    painter.save();
    painter.translate(m_boardOffset);
    if (isLegal)
    {
        BOOST_FOREACH(Point p, points)
        {
            qreal fieldX = p.get_x() * m_fieldWidth;
            qreal fieldY = (m_height - p.get_y() - 1) * m_fieldHeight;
            if (m_isTrigon)
            {
                bool isUpside = (m_geometry->get_point_type(p) == 1);
                Util::paintColorTriangle(painter, m_gameVariant, c, isUpside,
                                         fieldX, fieldY, m_fieldWidth,
                                         m_fieldHeight);
            }
            else
            {
                Util::paintColorSquare(painter, m_gameVariant, c, fieldX,
                                       fieldY, m_fieldWidth);
            }
        }
    }
    else
    {
        QColor color = Util::getPaintColor(m_gameVariant, c);
        color.setHsv(color.hue(), color.saturation() / 2, color.value());
        color.setAlpha(160);
        BOOST_FOREACH(Point p, points)
        {
            painter.save();
            painter.translate(p.get_x() * m_fieldWidth,
                              (m_height - p.get_y() - 1) * m_fieldHeight);
            if (m_isTrigon)
            {
                painter.setPen(Qt::NoPen);
                painter.setBrush(color);
                qreal left = -0.5 * m_fieldWidth;
                qreal right = 1.5 * m_fieldWidth;
                bool isUpside = (m_geometry->get_point_type(p) == 1);
                if (isUpside)
                {
                    const QPointF polygon[3] =
                        {
                            QPointF(left, m_fieldHeight),
                            QPointF(right, m_fieldHeight),
                            QPointF(0.5 * m_fieldWidth, 0)
                        };
                    painter.drawConvexPolygon(polygon, 3);
                }
                else
                {
                    const QPointF polygon[3] =
                        {
                            QPointF(left, 0),
                            QPointF(right, 0),
                            QPointF(0.5 * m_fieldWidth, m_fieldHeight)
                        };
                    painter.drawConvexPolygon(polygon, 3);
                }
            }
            else
            {
                painter.fillRect(QRectF(0, 0, m_fieldWidth, m_fieldHeight),
                                 color);
            }
            painter.restore();
        }
    }
    painter.restore();
}

//-----------------------------------------------------------------------------
