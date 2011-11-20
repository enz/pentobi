//-----------------------------------------------------------------------------
/** @file libpentobi_gui/BoardPainter.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardPainter.h"

#include <algorithm>
#include "Util.h"
#include "libpentobi_base/AdjIterator.h"
#include "libpentobi_base/DiagIterator.h"

using namespace std;
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

void BoardPainter::drawLabel(QPainter& painter, int x, int y,
                             const QString& label, bool underline,
                             bool smallFont)
{
    if (underline)
        painter.setFont(m_fontUnderlined);
    else if (smallFont)
        painter.setFont(m_fontSmall);
    else
        painter.setFont(m_font);
    int fieldX = x * m_fieldWidth;
    int fieldY = (m_height - y - 1) * m_fieldHeight;
    painter.drawText(fieldX, fieldY, m_fieldWidth, m_fieldHeight,
                     Qt::AlignCenter, label);
}

void BoardPainter::drawSelectedPiece(QPainter& painter, GameVariant gameVariant,
                                     const Geometry& geometry,
                                     const FullGrid<PointStateExt>& pointState)
{
    if (m_selectedPiecePoints.empty())
        return;
    if (m_isSelectedPieceLegal)
    {
        BOOST_FOREACH(Point p, m_selectedPiecePoints)
        {
            qreal fieldX = p.get_x() * m_fieldWidth;
            qreal fieldY = (m_height - p.get_y() - 1) * m_fieldHeight;
            if (gameVariant == game_variant_trigon
                || gameVariant == game_variant_trigon_2)
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
            painter.fillRect(QRectF(p.get_x() * m_fieldWidth,
                                    (m_height - p.get_y() - 1) * m_fieldHeight,
                                    m_fieldWidth, m_fieldHeight),
                             color);
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
    int extraSpace = 2;
    if (gameVariant == game_variant_trigon
        || gameVariant == game_variant_trigon_2)
        return QRect(round(m_boardOffset.x() + x * m_fieldWidth
                           - 0.5 * m_fieldWidth - extraSpace),
                     round(m_boardOffset.y() + y * m_fieldHeight - extraSpace),
                     round(2 * m_fieldWidth + 2 * extraSpace),
                     round(m_fieldHeight + 2 * extraSpace));
    else
        return QRect(round(m_boardOffset.x() + x * m_fieldWidth - extraSpace),
                     round(m_boardOffset.y() + y * m_fieldHeight - extraSpace),
                     round(m_fieldWidth + 2 * extraSpace),
                     round(m_fieldHeight + 2 * extraSpace));
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
    if (gameVariant == game_variant_trigon
        || gameVariant == game_variant_trigon_2)
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
    m_font.setPointSizeF(0.4 * m_fieldWidth);
    m_fontUnderlined = m_font;
    m_fontUnderlined.setUnderline(true);
    m_fontSmall.setPointSizeF(0.36 * m_fieldWidth);
    painter.save();
    painter.translate(m_boardOffset);
    if (m_drawCoordLabels)
    {
        painter.setPen(m_coordLabelColor);
        painter.setFont(m_font);
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
            drawLabel(painter, x, -1, label, false, true);
            drawLabel(painter, x, m_height, label, false, true);
        }
        for (int y = 0; y < m_height; ++y)
        {
            QString label;
            label.setNum(y + 1);
            drawLabel(painter, -1, y, label, false, true);
            drawLabel(painter, m_width, y, label, false, true);
        }
    }
    m_startingPoints.init(gameVariant, geometry);
    for (GeometryIterator i(geometry); i; ++i)
    {
        int x = i->get_x();
        int y = i->get_y();
        PointStateExt s = pointState[*i];
        qreal fieldX = x * m_fieldWidth;
        qreal fieldY = (m_height - y - 1) * m_fieldHeight;
        if (gameVariant == game_variant_trigon
            || gameVariant == game_variant_trigon_2)
        {
            bool isUpside = (geometry.get_point_type(x, y) == 1);
            if (s.is_color())
                Util::paintColorTriangle(painter, gameVariant, s.to_color(),
                                         isUpside, fieldX, fieldY,
                                         m_fieldWidth, m_fieldHeight);
            else
                Util::paintEmptyTriangle(painter, isUpside, fieldX, fieldY,
                                         m_fieldWidth, m_fieldHeight);
        }
        else
        {
            if (s.is_color())
                Util::paintColorSquare(painter, gameVariant, s.to_color(),
                                       fieldX, fieldY, m_fieldWidth);
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
                    Util::paintEmptySquare(painter, fieldX, fieldY,
                                           m_fieldWidth);
            }
        }
    }
    if (labels != 0)
    {
        for (GeometryIterator i(geometry); i; ++i)
            if (! (*labels)[*i].isEmpty())
            {
                PointState s = pointState[*i].to_point_state();
                painter.setPen(Util::getLabelColor(gameVariant, s));
                bool underline = false;
                if (markupFlags != 0
                    && (*markupFlags)[*i].test(markup_variation))
                    underline = true;
                drawLabel(painter, i->get_x(), i->get_y(), (*labels)[*i],
                          underline);
            }
    }
    drawSelectedPiece(painter, gameVariant, geometry, pointState);
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
