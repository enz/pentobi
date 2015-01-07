//-----------------------------------------------------------------------------
/** @file libpentobi_gui/BoardPainter.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardPainter.h"

#include <algorithm>
#include <cmath>
#include "Util.h"
#include "libboardgame_util/Log.h"

using namespace std;
using libboardgame_util::log;
using libpentobi_base::GeometryIterator;
using libpentobi_base::Move;
using libpentobi_base::PointState;

//-----------------------------------------------------------------------------

BoardPainter::BoardPainter()
    : m_hasPainted(false),
      m_coordinates(false),
      m_coordinateColor(Qt::black)
{
    m_font.setFamily("Helvetica");
    m_font.setStyleHint(QFont::SansSerif);
    m_font.setStyleStrategy(QFont::PreferOutline);
    m_fontSemiCondensed = m_font;
    m_fontSemiCondensed.setStretch(QFont::SemiCondensed);
    m_fontCondensed = m_font;
    m_fontCondensed.setStretch(QFont::Condensed);
    m_fontCoordLabels = m_font;
    m_fontCoordLabels.setStretch(QFont::SemiCondensed);
}

BoardPainter::~BoardPainter()
{
}

void BoardPainter::drawCoordinates(QPainter& painter, bool isTrigon)
{
    painter.setPen(m_coordinateColor);
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
                  m_fieldWidth, m_fieldHeight, label, true);
        drawLabel(painter, x * m_fieldWidth, -m_fieldHeight,
                  m_fieldWidth, m_fieldHeight, label, true);
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
                  m_fieldWidth, m_fieldHeight, label, true);
        drawLabel(painter, right, (m_height - y - 1) * m_fieldHeight,
                  m_fieldWidth, m_fieldHeight, label, true);
    }
}

void BoardPainter::drawLabel(QPainter& painter, qreal x, qreal y,
                             qreal width, qreal height, const QString& label,
                             bool isCoordLabel)
{
    if (isCoordLabel)
        painter.setFont(m_fontCoordLabels);
    else
        painter.setFont(m_font);
    QFontMetrics metrics(painter.font());
    QRect boundingRect = metrics.boundingRect(label);
    if (! isCoordLabel)
    {
        if (boundingRect.width() > width)
        {
            painter.setFont(m_fontSemiCondensed);
            QFontMetrics metrics(painter.font());
            boundingRect = metrics.boundingRect(label);
        }
        if (boundingRect.width() > width)
        {
            painter.setFont(m_fontCondensed);
            QFontMetrics metrics(painter.font());
            boundingRect = metrics.boundingRect(label);
        }
    }
    qreal dx = 0.5 * (width - boundingRect.width());
    qreal dy = 0.5 * (height - boundingRect.height());
    QRectF rect;
    rect.setCoords(floor(x + dx), floor(y + dy),
                   ceil(x + width - dx + 1), ceil(y + height - dy + 1));
    painter.drawText(rect, Qt::TextDontClip, label);
}

void BoardPainter::drawLabels(QPainter& painter,
                              const Grid<PointState>& pointState,
                              Variant variant,
                              const Grid<QString>* labels)
{
    if (! labels)
        return;
    bool isTrigon = (variant == Variant::trigon
                     || variant == Variant::trigon_2
                     || variant == Variant::trigon_3);
    for (GeometryIterator i(*m_geo); i; ++i)
        if (! (*labels)[*i].isEmpty())
        {
            PointState s = pointState[*i];
            painter.setPen(Util::getLabelColor(variant, s));
            qreal x = i->get_x(m_geo->get_width()) * m_fieldWidth;
            qreal y = i->get_y(m_geo->get_width()) * m_fieldHeight;
            qreal width = m_fieldWidth;
            qreal height = m_fieldHeight;
            if (isTrigon)
            {
                bool isUpward = (m_geo->get_point_type(*i) == 0);
                if (isUpward)
                    y += 0.333 * height;
                height = 0.666 * height;
            }
            drawLabel(painter, x, y, width, height, (*labels)[*i], false);
        }
}

CoordPoint BoardPainter::getCoordPoint(int x, int y)
{
    if (! m_hasPainted)
        return CoordPoint::null();
    x = static_cast<int>((x - m_boardOffset.x()) / m_fieldWidth);
    y = static_cast<int>((y - m_boardOffset.y()) / m_fieldHeight);
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return CoordPoint::null();
    else
        return CoordPoint(x, y);
}

void BoardPainter::paintEmptyBoard(QPainter& painter, unsigned width,
                                   unsigned height, Variant variant,
                                   const Geometry& geo)
{
    m_hasPainted = true;
    painter.setRenderHint(QPainter::Antialiasing, true);
    m_variant = variant;
    m_geo = &geo;
    m_width = static_cast<int>(m_geo->get_width());
    m_height = static_cast<int>(m_geo->get_height());
    m_isTrigon = (variant == Variant::trigon
                  || variant == Variant::trigon_2
                  || variant == Variant::trigon_3);
    qreal ratio;
    if (m_isTrigon)
    {
        ratio = 1.732;
        if (m_coordinates)
            m_fieldWidth =
                min(qreal(width) / (m_width + 3),
                    height / (ratio * (m_height + 2)));
        else
            m_fieldWidth =
                min(qreal(width) / (m_width + 1), height / (ratio * m_height));
    }
    else
    {
        ratio = 1;
        if (m_coordinates)
            m_fieldWidth =
                min(qreal(width) / (m_width + 2),
                    qreal(height) / (m_height + 2));
        else
            m_fieldWidth =
                min(qreal(width) / m_width, qreal(height) / m_height);
    }
    if (m_fieldWidth > 8)
        // Prefer pixel alignment if board is not too small
        m_fieldWidth = floor(m_fieldWidth);
    m_fieldHeight = ratio * m_fieldWidth;
    m_boardOffset = QPointF(0.5 * (width - m_fieldWidth * m_width),
                            0.5 * (height - m_fieldHeight * m_height));
    if (m_isTrigon)
    {
        m_font.setPointSizeF(0.6 * m_fieldWidth);
        m_fontSemiCondensed.setPointSizeF(0.6 * m_fieldWidth);
        m_fontCondensed.setPointSizeF(0.6 * m_fieldWidth);
        m_fontCoordLabels.setPointSizeF(0.6 * m_fieldWidth);
    }
    else
    {
        m_font.setPointSizeF(0.4 * m_fieldWidth);
        m_fontSemiCondensed.setPointSizeF(0.4 * m_fieldWidth);
        m_fontCondensed.setPointSizeF(0.4 * m_fieldWidth);
        m_fontCoordLabels.setPointSizeF(0.36 * m_fieldWidth);
    }
    painter.save();
    painter.translate(m_boardOffset);
    if (m_coordinates)
        drawCoordinates(painter, m_isTrigon);
    m_startingPoints.init(variant, *m_geo);
    auto geoWidth = m_geo->get_width();
    for (GeometryIterator i(*m_geo); i; ++i)
    {
        int x = i->get_x(geoWidth);
        int y = i->get_y(geoWidth);
        qreal fieldX = x * m_fieldWidth;
        qreal fieldY = y * m_fieldHeight;
        if (m_isTrigon)
        {
            bool isUpward = (m_geo->get_point_type(x, y) == 0);
            if (m_startingPoints.is_colorless_starting_point(*i))
                Util::paintEmptyTriangleStartingPoint(painter, isUpward, fieldX,
                                                      fieldY, m_fieldWidth,
                                                      m_fieldHeight);
            else
                Util::paintEmptyTriangle(painter, isUpward, fieldX, fieldY,
                                         m_fieldWidth, m_fieldHeight);
        }
        else
        {
            if (m_startingPoints.is_colored_starting_point(*i))
            {
                Color color = m_startingPoints.get_starting_point_color(*i);
                Util::paintEmptySquareStartingPoint(painter, variant,
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
                               const Grid<PointState>& pointState,
                               const Grid<QString>* labels)
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.save();
    painter.translate(m_boardOffset);
    auto geoWidth = m_geo->get_width();
    for (GeometryIterator i(*m_geo); i; ++i)
    {
        int x = i->get_x(geoWidth);
        int y = i->get_y(geoWidth);
        PointState s = pointState[*i];
        qreal fieldX = x * m_fieldWidth;
        qreal fieldY = y * m_fieldHeight;
        if (m_isTrigon)
        {
            bool isUpward = (m_geo->get_point_type(x, y) == 0);
            if (s.is_color())
                Util::paintColorTriangle(painter, m_variant, s.to_color(),
                                         isUpward, fieldX, fieldY,
                                         m_fieldWidth, m_fieldHeight);
        }
        else
        {
            if (s.is_color())
                Util::paintColorSquare(painter, m_variant, s.to_color(),
                                       fieldX, fieldY, m_fieldWidth);
        }
    }
    drawLabels(painter, pointState, m_variant, labels);
    painter.restore();
}

void BoardPainter::paintSelectedPiece(QPainter& painter, Color c,
                                      const MovePoints& points, bool isLegal)
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.save();
    painter.translate(m_boardOffset);
    qreal alpha;
    qreal saturation;
    bool flat;
    if (isLegal)
    {
        alpha = 0.9;
        saturation = 0.8;
        flat = false;
    }
    else
    {
        alpha = 0.63;
        saturation = 0.5;
        flat = true;
    }
    auto geoWidth = m_geo->get_width();
    for (Point p : points)
    {
        qreal fieldX = p.get_x(geoWidth) * m_fieldWidth;
        qreal fieldY = p.get_y(geoWidth) * m_fieldHeight;
        if (m_isTrigon)
        {
            bool isUpward = (m_geo->get_point_type(p) == 0);
            Util::paintColorTriangle(painter, m_variant, c, isUpward,
                                     fieldX, fieldY, m_fieldWidth,
                                     m_fieldHeight, alpha, saturation, flat);
        }
        else
        {
            Util::paintColorSquare(painter, m_variant, c, fieldX, fieldY,
                                   m_fieldWidth, alpha, saturation, flat);
        }
    }
    painter.restore();
}

//-----------------------------------------------------------------------------
