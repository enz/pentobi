//-----------------------------------------------------------------------------
/** @file libpentobi_gui/BoardPainter.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardPainter.h"

#include <algorithm>
#include "Util.h"
#include "libboardgame_util/Log.h"
#include "libpentobi_base/AdjIterator.h"
#include "libpentobi_base/DiagIterator.h"

using namespace std;
using libboardgame_util::log;
using libpentobi_base::game_variant_trigon;
using libpentobi_base::game_variant_trigon_2;
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
                   ceil(x + width - dx), ceil(y + height - dy));
    painter.drawText(rect, label);
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
                     || gameVariant == game_variant_trigon_2);
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

void BoardPainter::drawSelectedPiece(QPainter& painter, GameVariant gameVariant,
                                     const FullGrid<PointStateExt>& pointState)
{
    if (m_selectedPiecePoints.empty())
        return;
    const Geometry& geometry = pointState.get_geometry();
    bool isTrigon = (gameVariant == game_variant_trigon
                     || gameVariant == game_variant_trigon_2);
    if (m_isSelectedPieceLegal)
    {
        BOOST_FOREACH(Point p, m_selectedPiecePoints)
        {
            qreal fieldX = p.get_x() * m_fieldWidth;
            qreal fieldY = (m_height - p.get_y() - 1) * m_fieldHeight;
            if (isTrigon)
            {
                bool isUpside = (geometry.get_point_type(p) == 1);
                Util::paintColorTriangle(painter, gameVariant,
                                         m_selectedPieceColor, isUpside, fieldX,
                                         fieldY, m_fieldWidth, m_fieldHeight);
            }
            else
            {
                Util::paintColorSquare(painter, gameVariant,
                                       m_selectedPieceColor, fieldX, fieldY,
                                       m_fieldWidth);
            }
        }
    }
    else
    {
        QColor color = Util::getPaintColor(gameVariant, m_selectedPieceColor);
        color.setHsv(color.hue(), color.saturation() / 2, color.value());
        color.setAlpha(160);
        BOOST_FOREACH(Point p, m_selectedPiecePoints)
        {
            painter.save();
            painter.translate(p.get_x() * m_fieldWidth,
                              (m_height - p.get_y() - 1) * m_fieldHeight);
            if (isTrigon)
            {
                painter.setPen(Qt::NoPen);
                painter.setBrush(color);
                qreal left = -0.5 * m_fieldWidth;
                qreal right = 1.5 * m_fieldWidth;
                bool isUpside = (geometry.get_point_type(p) == 1);
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

QRect BoardPainter::getRect(Point p, GameVariant gameVariant) const
{
    if (! m_hasPainted)
        return QRect();
    int x = p.get_x();
    int y = m_height - p.get_y() - 1;
    // Uses some extra space because overlong labels and antialiasing can
    // affect neighboring fields
    qreal extraWidth = min(0.5 * m_fieldWidth, 1.);
    qreal extraHeight = min(0.2 * m_fieldHeight, 1.);
    if (gameVariant == game_variant_trigon
        || gameVariant == game_variant_trigon_2)
        return QRect(round(m_boardOffset.x() + (x - 0.5) * m_fieldWidth
                           - extraWidth),
                     round(m_boardOffset.y() + y * m_fieldHeight - extraHeight),
                     round(2 * m_fieldWidth + 2 * extraWidth),
                     round(m_fieldHeight + 2 * extraHeight));
    else
        return QRect(round(m_boardOffset.x() + x * m_fieldWidth - extraWidth),
                     round(m_boardOffset.y() + y * m_fieldHeight - extraHeight),
                     round(2 * m_fieldWidth + 2 * extraWidth),
                     round(m_fieldHeight + 2 * extraHeight));
}

void BoardPainter::paint(QPainter& painter, unsigned int width,
                         unsigned int height, GameVariant gameVariant,
                         const FullGrid<PointStateExt>& pointState,
                         const Grid<QString>* labels,
                         const Grid<MarkupFlags>* markupFlags)
{
    m_hasPainted = true;
    painter.setRenderHint(QPainter::Antialiasing, true);
    const Geometry& geometry = pointState.get_geometry();
    m_width = static_cast<int>(geometry.get_width());
    m_height = static_cast<int>(geometry.get_height());
    bool isTrigon = (gameVariant == game_variant_trigon
                     || gameVariant == game_variant_trigon_2);
    if (isTrigon)
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
    if (isTrigon)
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
        drawCoordinates(painter, isTrigon);
    m_startingPoints.init(gameVariant, geometry);
    for (GeometryIterator i(geometry); i; ++i)
    {
        int x = i->get_x();
        int y = i->get_y();
        PointStateExt s = pointState[*i];
        qreal fieldX = x * m_fieldWidth;
        qreal fieldY = (m_height - y - 1) * m_fieldHeight;
        if (isTrigon)
        {
            bool isUpside = (geometry.get_point_type(x, y) == 1);
            if (s.is_color())
                Util::paintColorTriangle(painter, gameVariant, s.to_color(),
                                         isUpside, fieldX, fieldY,
                                         m_fieldWidth, m_fieldHeight);
            else if (m_startingPoints.is_colorless_starting_point(*i))
                Util::paintEmptyTriangleStartingPoint(painter, isUpside, fieldX,
                                                      fieldY, m_fieldWidth,
                                                      m_fieldHeight);
            else
                Util::paintEmptyTriangle(painter, isUpside, fieldX, fieldY,
                                         m_fieldWidth, m_fieldHeight);
        }
        else
        {
            if (s.is_color())
                Util::paintColorSquare(painter, gameVariant, s.to_color(),
                                       fieldX, fieldY, m_fieldWidth);
            else if (m_startingPoints.is_colored_starting_point(*i))
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
    drawLabels(painter, pointState, gameVariant, labels, markupFlags);
    drawSelectedPiece(painter, gameVariant, pointState);
    painter.restore();
}

void BoardPainter::setSelectedPiece(Color c, const MovePoints& points,
                                    bool isLegal)
{
    m_selectedPieceColor = c;
    m_selectedPiecePoints = points;
    m_isSelectedPieceLegal = isLegal;
}

//-----------------------------------------------------------------------------
