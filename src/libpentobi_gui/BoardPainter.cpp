//-----------------------------------------------------------------------------
/** @file libpentobi_gui/BoardPainter.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardPainter.h"

#include <algorithm>
#include "Util.h"

using namespace std;
using libpentobi_base::BoardIterator;
using libpentobi_base::Color;
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
        LIBBOARDGAME_FOREACH_ADJ(p, pAdj,
            if (pointState[pAdj] == c)
                return false; )
        if (isFirstPiece && p == startingPoint)
            isConnected = true;
        LIBBOARDGAME_FOREACH_DIAG(p, pDiag,
            if (pointState[pDiag] == c)
                isConnected = true; )
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
                             const QString& label, bool underline)
{
    if (underline)
        painter.setFont(m_fontUnderlined);
    else
        painter.setFont(m_font);
    int squareX = x * m_squareSize;
    int squareY = (m_sz - y - 1) * m_squareSize;
    painter.drawText(squareX, squareY, m_squareSize, m_squareSize,
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
            int squareX = p.get_x() * m_squareSize;
            int squareY = (m_sz - p.get_y() - 1) * m_squareSize;
            Util::paintColorSquare(painter, gameVariant, m_selectedPieceColor,
                                   squareX, squareY, m_squareSize);
        }
    }
    else
    {
        QColor color = Util::getPaintColor(gameVariant, m_selectedPieceColor);
        color.setHsv(color.hue(), color.saturation() / 2, color.value());
        color.setAlpha(160);
        BOOST_FOREACH(Point p, m_selectedPiecePoints)
        {
            painter.fillRect(p.get_x() * m_squareSize,
                             (m_sz - p.get_y() - 1) * m_squareSize,
                             m_squareSize, m_squareSize, color);
        }
    }
}

CoordPoint BoardPainter::getCoordPoint(int x, int y)
{
    if (! m_hasPainted)
        return CoordPoint::null();
    int sz = static_cast<int>(m_sz);
    x = (x - m_boardOffset.x()) / m_squareSize;
    y = (y - m_boardOffset.y()) / m_squareSize;
    y = m_sz - y - 1;
    if (x < 0 || x >= sz || y < 0 || y >= sz)
        return CoordPoint::null();
    else
        return CoordPoint(x, y);
}

QRect BoardPainter::getRect(Point p) const
{
    if (! m_hasPainted)
        return QRect();
    int x = p.get_x();
    int y = m_sz - p.get_y() - 1;
    return QRect(m_boardOffset.x() + x * m_squareSize,
                 m_boardOffset.y() + y * m_squareSize,
                 m_squareSize, m_squareSize);
}

void BoardPainter::paint(QPainter& painter, unsigned int width,
                         unsigned int height, GameVariant gameVariant,
                         const FullGrid<PointStateExt>& pointState,
                         const Grid<QString>* labels,
                         const Grid<MarkupFlags>* markupFlags)
{
    m_hasPainted = true;
    m_sz = static_cast<int>(pointState.get_geometry().get_size());
    if (m_drawCoordLabels)
        m_squareSize = min(width, height) / (m_sz + 2);
    else
        m_squareSize = min(width, height) / m_sz;
    m_boardSize = m_squareSize * m_sz;
    m_font.setPointSize(max(m_squareSize * 40 / 100, 1));
    m_fontUnderlined = m_font;
    m_fontUnderlined.setUnderline(true);
    m_boardOffset =
        QPoint((width - m_boardSize) / 2, (height - m_boardSize) / 2);
    painter.save();
    painter.translate(m_boardOffset);
    if (m_drawCoordLabels)
    {
        painter.setPen(m_coordLabelColor);
        painter.setFont(m_font);
        for (int x = 0; x < m_sz; ++x)
        {
            QString label(QChar('A' + x));
            drawLabel(painter, x, -1, label, false);
            drawLabel(painter, x, m_sz, label, false);
        }
        for (int y = 0; y < m_sz; ++y)
        {
            QString label;
            label.setNum(y + 1);
            drawLabel(painter, -1, y, label, false);
            drawLabel(painter, m_sz, y, label, false);
        }
    }
    const Geometry& geometry = pointState.get_geometry();
    for (GeometryIterator i(geometry); i; ++i)
    {
        int x = i->get_x();
        int y = i->get_y();
        PointStateExt s = pointState[*i];
        int squareX = x * m_squareSize;
        int squareY = (m_sz - y - 1) * m_squareSize;
        if (s.is_color())
            Util::paintColorSquare(painter, gameVariant, s.to_color(),
                                   squareX, squareY, m_squareSize);
        else
        {
            Color color;
            if (Board::is_starting_point(*i, gameVariant, color))
                Util::paintEmptySquareStartingPoint(painter, gameVariant,
                                                    color, squareX, squareY,
                                                    m_squareSize);
            else
                Util::paintEmptySquare(painter, squareX, squareY, m_squareSize);
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
