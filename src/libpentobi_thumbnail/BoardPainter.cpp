//-----------------------------------------------------------------------------
/** @file libpentobi_gui/BoardPainter.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BoardPainter.h"
#include "libpentobi_base/CallistoGeometry.h"

#include <algorithm>
#include <cmath>
#include "Util.h"

using namespace std;
using libboardgame_util::ArrayList;
using libpentobi_base::CallistoGeometry;
using libpentobi_base::PieceSet;
using libpentobi_base::PointState;

//-----------------------------------------------------------------------------

void BoardPainter::paintEmptyBoard(QPainter& painter, unsigned width,
                                   unsigned height, Variant variant,
                                   const Geometry& geo)
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    m_variant = variant;
    auto pieceSet = get_piece_set(variant);
    m_geo = &geo;
    m_width = static_cast<int>(m_geo->get_width());
    m_height = static_cast<int>(m_geo->get_height());
    m_isTrigon = (pieceSet == PieceSet::trigon);
    m_isNexos = (pieceSet == PieceSet::nexos);
    m_isCallisto = (pieceSet == PieceSet::callisto);
    m_isGembloQ = (pieceSet == PieceSet::gembloq);
    qreal ratio;
    if (m_isTrigon)
    {
        ratio = 1.732;
        m_fieldWidth =
                min(qreal(width) / (m_width + 1), height / (ratio * m_height));
    }
    else if (m_isGembloQ)
    {
        ratio = 2;
        m_fieldWidth =
                min(qreal(width) / m_width, qreal(height) / (ratio * m_height));
    }
    else
    {
        ratio = 1;
        m_fieldWidth = min(qreal(width) / m_width, qreal(height) / m_height);
    }
    if (m_fieldWidth > 8)
        // Prefer pixel alignment if board is not too small
        m_fieldWidth = floor(m_fieldWidth);
    m_fieldHeight = ratio * m_fieldWidth;
    m_boardOffset = QPointF(0.5 * (width - m_fieldWidth * m_width),
                            0.5 * (height - m_fieldHeight * m_height));
    painter.save();
    painter.translate(m_boardOffset);
    if (m_isNexos)
        painter.fillRect(QRectF(m_fieldWidth / 4, m_fieldHeight / 4,
                                m_width * m_fieldWidth - m_fieldWidth / 2,
                                m_height * m_fieldHeight - m_fieldHeight / 2),
                         QColor(174, 167, 172));
    auto nu_colors = get_nu_colors(m_variant);
    for (Point p : *m_geo)
    {
        int x = m_geo->get_x(p);
        int y = m_geo->get_y(p);
        qreal fieldX = x * m_fieldWidth;
        qreal fieldY = y * m_fieldHeight;
        auto pointType = m_geo->get_point_type(p);
        if (m_isTrigon)
        {
            bool isUpward = (pointType == 0);
            Util::paintEmptyTriangle(painter, isUpward, fieldX, fieldY,
                                     m_fieldWidth, m_fieldHeight);
        }
        else if (m_isNexos)
        {
            if (pointType == 1 || pointType == 2)
            {
                bool isHorizontal = (pointType == 1);
                Util::paintEmptySegment(painter, isHorizontal, fieldX, fieldY,
                                        m_fieldWidth);
            }
            else
            {
                LIBBOARDGAME_ASSERT(pointType == 0);
                Util::paintEmptyJunction(painter, fieldX, fieldY,
                                         m_fieldWidth);
            }
        }
        else if (m_isGembloQ)
            Util::paintEmptyGembloQ(painter, pointType, fieldX, fieldY,
                                    m_fieldWidth);
        else if (m_isCallisto
                 && CallistoGeometry::is_center_section(x, y, nu_colors))
            Util::paintEmptySquareCallistoCenter(painter, fieldX, fieldY,
                                                 m_fieldWidth);
        else if (m_isCallisto)
            Util::paintEmptySquareCallisto(painter, fieldX, fieldY,
                                           m_fieldWidth);
        else
            Util::paintEmptySquare(painter, fieldX, fieldY, m_fieldWidth);
    }
    painter.restore();
}

void BoardPainter::paintJunction(QPainter& painter, Variant variant,
                                 const Grid<PointState>& pointState,
                                 const Grid<unsigned>& pieceId, int x, int y,
                                 qreal fieldX, qreal fieldY)
{
    LIBBOARDGAME_ASSERT(m_geo->get_point_type(x, y) == 0);
    ArrayList<unsigned, 4> pieces;
    if (x > 0)
    {
        auto piece = pieceId[m_geo->get_point(x - 1, y)];
        if (piece != 0)
            pieces.include(piece);
    }
    if (x < m_width - 1)
    {
        auto piece = pieceId[m_geo->get_point(x + 1, y)];
        if (piece != 0)
            pieces.include(piece);
    }
    if (y > 0)
    {
        auto piece = pieceId[m_geo->get_point(x, y - 1)];
        if (piece != 0)
            pieces.include(piece);
    }
    if (y < m_height - 1)
    {
        auto piece = pieceId[m_geo->get_point(x, y + 1)];
        if (piece != 0)
            pieces.include(piece);
    }
    for (auto piece : pieces)
    {
        Color c;
        bool hasLeft = false;
        if (x > 0)
        {
            Point p = m_geo->get_point(x - 1, y);
            if (pieceId[p] == piece)
            {
                hasLeft = true;
                c = pointState[p].to_color();
            }
        }
        bool hasRight = false;
        if (x < m_width - 1)
        {
            Point p = m_geo->get_point(x + 1, y);
            if (pieceId[p] == piece)
            {
                hasRight = true;
                c = pointState[p].to_color();
            }
        }
        bool hasUp = false;
        if (y > 0)
        {
            Point p = m_geo->get_point(x, y - 1);
            if (pieceId[p] == piece)
            {
                hasUp = true;
                c = pointState[p].to_color();
            }
        }
        bool hasDown = false;
        if (y < m_height - 1)
        {
            Point p = m_geo->get_point(x, y + 1);
            if (pieceId[p] == piece)
            {
                hasDown = true;
                c = pointState[p].to_color();
            }
        }
        Util::paintJunction(painter, variant, c, fieldX, fieldY, m_fieldWidth,
                            m_fieldHeight, hasLeft, hasRight, hasUp, hasDown);
    }
}

void BoardPainter::paintPieces(QPainter& painter,
                               const Grid<PointState>& pointState,
                               const Grid<unsigned>& pieceId)
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.save();
    painter.translate(m_boardOffset);
    ColorMap<bool> isFirstPiece(true);
    for (Point p : *m_geo)
    {
        int x = m_geo->get_x(p);
        int y = m_geo->get_y(p);
        PointState s = pointState[p];
        qreal fieldX = x * m_fieldWidth;
        qreal fieldY = y * m_fieldHeight;
        auto pointType = m_geo->get_point_type(p);
        if (m_isTrigon)
        {
            if (s.is_empty())
                continue;
            Color c = s.to_color();
            isFirstPiece[c] = false;
            bool isUpward = (pointType == 0);
            Util::paintColorTriangle(painter, m_variant, c, isUpward, fieldX,
                                     fieldY, m_fieldWidth, m_fieldHeight);
        }
        else if (m_isNexos)
        {
            if (pointType == 1 || pointType == 2)
            {
                if (s.is_empty())
                    continue;
                Color c = s.to_color();
                isFirstPiece[c] = false;
                bool isHorizontal = (pointType == 1);
                Util::paintColorSegment(painter, m_variant, c, isHorizontal,
                                        fieldX, fieldY, m_fieldWidth);
            }
            else
            {
                LIBBOARDGAME_ASSERT(pointType == 0);
                paintJunction(painter, m_variant, pointState, pieceId, x, y,
                              fieldX, fieldY);
            }
        }
        else if (m_isGembloQ)
        {
            if (s.is_empty())
                continue;
            Color c = s.to_color();
            isFirstPiece[c] = false;
            Util::paintColorGembloQ(painter, m_variant, c, pointType, fieldX,
                                    fieldY, m_fieldWidth);
        }
        else
        {
            if (s.is_empty())
                continue;
            Color c = s.to_color();
            isFirstPiece[c] = false;
            if (m_isCallisto)
            {
                bool hasLeft =
                        (x > 0 && m_geo->is_onboard(x - 1, y)
                         && pieceId[p] == pieceId[m_geo->get_point(x - 1, y)]);
                bool hasRight =
                        (x < m_width - 1 && m_geo->is_onboard(x + 1, y)
                         && pieceId[p] == pieceId[m_geo->get_point(x + 1, y)]);
                bool hasUp =
                        (y > 0 && m_geo->is_onboard(x, y - 1)
                         && pieceId[p] == pieceId[m_geo->get_point(x, y - 1)]);
                bool hasDown =
                        (y < m_height - 1 && m_geo->is_onboard(x, y + 1)
                         && pieceId[p] == pieceId[m_geo->get_point(x, y + 1)]);
                bool isOnePiece =
                        (! hasLeft && ! hasRight && ! hasUp && ! hasDown);
                Util::paintColorSquareCallisto(painter, m_variant, c, fieldX,
                                               fieldY, m_fieldWidth, hasRight,
                                               hasDown, isOnePiece);
            }
            else
                Util::paintColorSquare(painter, m_variant, c, fieldX, fieldY,
                                       m_fieldWidth);
        }
    }
    painter.restore();
}

//-----------------------------------------------------------------------------
