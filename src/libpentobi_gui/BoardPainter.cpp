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
using libpentobi_base::AdjIterator;
using libpentobi_base::BoardIterator;
using libpentobi_base::Color;
using libpentobi_base::DiagIterator;
using libpentobi_base::FullGrid;
using libpentobi_base::Geometry;
using libpentobi_base::GeometryIterator;
using libpentobi_base::Move;
using libpentobi_base::MovePoints;
using libpentobi_base::Piece;
using libpentobi_base::Point;
using libpentobi_base::PointState;
using libpentobi_base::PointStateExt;

//-----------------------------------------------------------------------------

namespace {

bool isLegal(const MovePoints& movePoints, Color c, GameVariant gameVariant,
             const FullGrid<PointStateExt>& pointState)
{
    bool isFirstPiece = true;
    const Geometry& geometry = pointState.get_geometry();
    for (GeometryIterator i(geometry); i; ++i)
        if (pointState[*i] == c)
        {
            isFirstPiece = false;
            break;
        }
    Point startingPoint = Board::get_starting_point(gameVariant, c);
    bool isConnected = false;
    BOOST_FOREACH(Point p, movePoints)
    {
        if (! geometry.is_onboard(p) || ! pointState[p].is_empty())
            return false;
        for (AdjIterator i(geometry, p); i; ++i)
            if (pointState[*i] == c)
                return false;
        if (isFirstPiece && p == startingPoint)
            isConnected = true;
        for (DiagIterator i(geometry, p); i; ++i)
            if (pointState[*i] == c)
                isConnected = true;
    }
    return isConnected;
}

} // namespace

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
                                     const FullGrid<PointStateExt>& pointState)
{
    if (m_selectedPiecePoints.empty())
        return;
    if (isLegal(m_selectedPiecePoints, m_selectedPieceColor, gameVariant,
                pointState))
    {
        BOOST_FOREACH(Point p, m_selectedPiecePoints)
        {
            int fieldX = p.get_x() * m_fieldWidth;
            int fieldY = (m_height - p.get_y() - 1) * m_fieldHeight;
            Util::paintColorSquare(painter, gameVariant, m_selectedPieceColor,
                                   fieldX, fieldY, m_fieldWidth);
        }
    }
    else
    {
        QColor color = Util::getPaintColor(gameVariant, m_selectedPieceColor);
        color.setHsv(color.hue(), color.saturation() / 2, color.value());
        color.setAlpha(160);
        BOOST_FOREACH(Point p, m_selectedPiecePoints)
        {
            painter.fillRect(p.get_x() * m_fieldWidth,
                             (m_height - p.get_y() - 1) * m_fieldHeight,
                             m_fieldWidth, m_fieldHeight, color);
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

QRect BoardPainter::getRect(Point p) const
{
    if (! m_hasPainted)
        return QRect();
    int x = p.get_x();
    int y = m_height - p.get_y() - 1;
    return QRect(m_boardOffset.x() + x * m_fieldWidth,
                 m_boardOffset.y() + y * m_fieldHeight,
                 m_fieldWidth, m_fieldHeight);
}

void BoardPainter::paint(QPainter& painter, unsigned int width,
                         unsigned int height, GameVariant gameVariant,
                         const FullGrid<PointStateExt>& pointState,
                         const Grid<QString>* labels,
                         const Grid<MarkupFlags>* markupFlags)
{
    m_hasPainted = true;
    const Geometry& geometry = pointState.get_geometry();
    m_width = static_cast<int>(geometry.get_width());
    m_height = static_cast<int>(geometry.get_height());
    if (m_drawCoordLabels)
    {
        m_fieldWidth = min(width / (m_width + 2), height / (m_height + 2));
        m_fieldHeight = m_fieldWidth;
    }
    else
    {
        m_fieldWidth = min(width / m_width, height / m_height);
        m_fieldHeight = m_fieldWidth;
    }
    m_boardWidth = m_fieldWidth * m_width;
    m_boardHeight = m_fieldHeight * m_height;
    m_font.setPointSize(max(m_fieldWidth * 40 / 100, 1));
    m_fontUnderlined = m_font;
    m_fontUnderlined.setUnderline(true);
    m_fontSmall.setPointSize(max(m_fieldWidth * 34 / 100, 1));
    m_boardOffset =
        QPoint((width - m_boardWidth) / 2, (height - m_boardHeight) / 2);
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
    for (GeometryIterator i(geometry); i; ++i)
    {
        int x = i->get_x();
        int y = i->get_y();
        PointStateExt s = pointState[*i];
        int fieldX = x * m_fieldWidth;
        int fieldY = (m_height - y - 1) * m_fieldHeight;
        if (s.is_color())
            Util::paintColorSquare(painter, gameVariant, s.to_color(),
                                   fieldX, fieldY, m_fieldWidth);
        else
        {
            Color color;
            if (Board::is_starting_point(*i, gameVariant, color))
                Util::paintEmptySquareStartingPoint(painter, gameVariant,
                                                    color, fieldX, fieldY,
                                                    m_fieldWidth);
            else
                Util::paintEmptySquare(painter, fieldX, fieldY, m_fieldWidth);
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
    drawSelectedPiece(painter, gameVariant, pointState);
    painter.restore();
}

void BoardPainter::setSelectedPiece(Color c, const MovePoints& points)
{
    m_selectedPieceColor = c;
    m_selectedPiecePoints = points;
}

//-----------------------------------------------------------------------------
