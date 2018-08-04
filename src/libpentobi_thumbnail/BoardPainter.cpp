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

BoardPainter::BoardPainter()
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

CoordPoint BoardPainter::getCoordPoint(int x, int y)
{
    if (! m_hasPainted)
        return CoordPoint::null();
    x = static_cast<int>((x - m_boardOffset.x()) / m_fieldWidth);
    y = static_cast<int>((y - m_boardOffset.y()) / m_fieldHeight);
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return CoordPoint::null();
    return CoordPoint(x, y);
}

void BoardPainter::paintCoordinates(QPainter& painter)
{
    painter.setPen(m_coordinateColor);
    for (int x = 0; x < m_width; ++x)
    {
        QString label;
        if (x < 26)
            label = QString(QChar('A' + x));
        else
        {
            label = QString(QChar('A' + x / 26 - 1));
            label.append(QChar('A' + (x % 26)));
        }
        paintLabel(painter, x * m_fieldWidth, m_height * m_fieldHeight,
                   m_fieldWidth, m_fieldHeight, label, true);
        paintLabel(painter, x * m_fieldWidth, -m_fieldHeight,
                   m_fieldWidth, m_fieldHeight, label, true);
    }
    for (int y = 0; y < m_height; ++y)
    {
        QString label;
        label.setNum(y + 1);
        qreal left;
        qreal right;
        if (m_isTrigon)
        {
            left = -1.5 * m_fieldWidth;
            right = (m_width + 0.5) * m_fieldWidth;
        }
        else
        {
            left = -m_fieldWidth;
            right = m_width * m_fieldWidth;
        }
        paintLabel(painter, left, (m_height - y - 1) * m_fieldHeight,
                   m_fieldWidth, m_fieldHeight, label, true);
        paintLabel(painter, right, (m_height - y - 1) * m_fieldHeight,
                   m_fieldWidth, m_fieldHeight, label, true);
    }
}

void BoardPainter::paintEmptyBoard(QPainter& painter, unsigned width,
                                   unsigned height, Variant variant,
                                   const Geometry& geo)
{
    m_hasPainted = true;
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
        if (m_coordinates)
            m_fieldWidth =
                    min(qreal(width) / (m_width + 3),
                        height / (ratio * (m_height + 2)));
        else
            m_fieldWidth =
                    min(qreal(width) / (m_width + 1), height / (ratio * m_height));
    }
    else if (m_isGembloQ)
    {
        ratio = 2;
        if (m_coordinates)
            m_fieldWidth =
                    min(qreal(width) / (m_width + 2),
                        height / (ratio * (m_height + 2)));
        else
            m_fieldWidth =
                    min(qreal(width) / m_width, qreal(height) / (ratio * m_height));
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
    int fontSize;
    int coordFontSize;
    if (m_isTrigon)
    {
        fontSize = static_cast<int>(0.7f * m_fieldWidth);
        coordFontSize = fontSize;
    }
    else if (m_isGembloQ)
    {
        fontSize = static_cast<int>(1.4f * m_fieldWidth);
        coordFontSize = static_cast<int>(0.7f * m_fieldWidth);
    }
    else
    {
        fontSize = static_cast<int>(0.5f * m_fieldWidth);
        coordFontSize = fontSize;
    }
    // QFont::setPixelSize(0) prints a warning even if it works and the docs
    // of Qt 5.3 don't forbid it (unlike QFont::setPointSize(0))
    fontSize = max(fontSize, 1);
    coordFontSize = max(coordFontSize, 1);
    m_font.setPixelSize(fontSize);
    m_fontSemiCondensed.setPixelSize(fontSize);
    m_fontCondensed.setPixelSize(fontSize);
    m_fontCoordLabels.setPixelSize(coordFontSize);
    painter.save();
    painter.translate(m_boardOffset);
    if (m_coordinates)
        paintCoordinates(painter);
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

void BoardPainter::paintLabel(QPainter& painter, qreal x, qreal y,
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

void BoardPainter::paintLabels(QPainter& painter,
                               const Grid<PointState>& pointState,
                               Variant variant, const Grid<QString>& labels)
{
    for (Point p : *m_geo)
        if (! labels[p].isEmpty())
        {
            painter.setPen(Util::getLabelColor(variant, pointState[p]));
            qreal x = m_geo->get_x(p) * m_fieldWidth;
            qreal y = m_geo->get_y(p) * m_fieldHeight;
            qreal width = m_fieldWidth;
            qreal height = m_fieldHeight;
            if (m_isTrigon)
            {
                bool isUpward = (m_geo->get_point_type(p) == 0);
                if (isUpward)
                    y += 0.333 * height;
                height = 0.666 * height;
            }
            else if (m_isGembloQ)
            {
                switch (m_geo->get_point_type(p))
                {
                case 0:
                    x -= width / 2;
                    y -= height / 2;
                    break;
                case 1:
                    x += width / 2;
                    y += height / 2;
                    break;
                case 2:
                    x -= width / 2;
                    y += height / 2;
                    break;
                case 3:
                    x += width / 2;
                    y -= height / 2;
                    break;
                }
            }
            paintLabel(painter, x, y, width, height, labels[p], false);
        }
}

void BoardPainter::paintMarks(QPainter& painter,
                              const Grid<PointState>& pointState,
                              Variant variant, const Grid<int>& marks)
{
    for (Point p : *m_geo)
        if ((marks[p] & (dot | circle)) != 0)
        {
            qreal x = (static_cast<float>(m_geo->get_x(p)) + 0.5f)
                    * m_fieldWidth;
            qreal y = (static_cast<float>(m_geo->get_y(p)) + 0.5f)
                    * m_fieldHeight;
            qreal size;
            if (m_isTrigon)
            {
                bool isUpward = (m_geo->get_point_type(p) == 0);
                if (isUpward)
                    y += 0.167 * m_fieldHeight;
                else
                    y -= 0.167 * m_fieldHeight;
                size = 0.1 * m_fieldHeight;
            }
            else if (m_isGembloQ)
            {
                size = 0.4 * m_fieldWidth;
                switch (m_geo->get_point_type(p))
                {
                case 0:
                    x -= m_fieldWidth / 2;
                    y -= m_fieldHeight / 2;
                    break;
                case 1:
                    x += m_fieldWidth / 2;
                    y += m_fieldHeight / 2;
                    break;
                case 2:
                    x -= m_fieldWidth / 2;
                    y += m_fieldHeight / 2;
                    break;
                case 3:
                    x += m_fieldWidth / 2;
                    y -= m_fieldHeight / 2;
                    break;
                }
            }
            else if (m_isCallisto)
                size = 0.1 * m_fieldHeight;
            else
                size = 0.12 * m_fieldHeight;
            QColor color = Util::getMarkColor(variant, pointState[p]);
            qreal penWidth = 0.05 * m_fieldHeight;
            if ((marks[p] & dot) != 0)
            {
                color.setAlphaF(0.5);
                painter.setPen(Qt::NoPen);
                painter.setBrush(color);
                size *= (1 + 0.25 * penWidth);
            }
            else
            {
                color.setAlphaF(0.6);
                QPen pen(color);
                pen.setWidthF(penWidth);
                painter.setPen(pen);
                painter.setBrush(Qt::NoBrush);
            }
            painter.drawEllipse(QPointF(x, y), size, size);
        }
}

void BoardPainter::paintPieces(QPainter& painter,
                               const Grid<PointState>& pointState,
                               const Grid<unsigned>& pieceId,
                               const Grid<QString>* labels,
                               const Grid<int>* marks)
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
    paintStartingPoints(painter, m_variant, pointState, isFirstPiece);
    if (marks != nullptr)
        paintMarks(painter, pointState, m_variant, *marks);
    if (labels != nullptr)
        paintLabels(painter, pointState, m_variant, *labels);
    painter.restore();
}

void BoardPainter::paintSelectedPiece(QPainter& painter, Color c,
                                      const MovePoints& points,
                                      bool isOnePiece, bool isLegal)
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
    ArrayList<Point, 2 * PieceInfo::max_size> junctions;
    for (Point p : points)
    {
        if (p.is_null())
            continue;
        int x = m_geo->get_x(p);
        int y = m_geo->get_y(p);
        auto pointType = m_geo->get_point_type(p);
        qreal fieldX = x * m_fieldWidth;
        qreal fieldY = y * m_fieldHeight;
        if (m_isTrigon)
        {
            bool isUpward = (pointType == 0);
            Util::paintColorTriangle(painter, m_variant, c, isUpward,
                                     fieldX, fieldY, m_fieldWidth,
                                     m_fieldHeight, alpha, saturation, flat);
        }
        else if (m_isNexos)
        {
            if (pointType == 1 || pointType == 2)
            {
                bool isHorizontal = (pointType == 1);
                Util::paintColorSegment(painter, m_variant, c, isHorizontal,
                                        fieldX, fieldY, m_fieldWidth, alpha,
                                        saturation, flat);
                if (isHorizontal)
                {
                    if (m_geo->is_onboard(x - 1, y))
                        junctions.include(m_geo->get_point(x - 1, y));
                    if (m_geo->is_onboard(x + 1, y))
                        junctions.include(m_geo->get_point(x + 1, y));
                }
                else
                {
                    if (m_geo->is_onboard(x, y - 1))
                        junctions.include(m_geo->get_point(x, y - 1));
                    if (m_geo->is_onboard(x, y + 1))
                        junctions.include(m_geo->get_point(x, y + 1));
                }
            }
        }
        else if (m_isGembloQ)
            Util::paintColorGembloQ(painter, m_variant, c, pointType,
                                     fieldX, fieldY, m_fieldWidth,
                                     alpha, saturation, flat);
        else if (m_isCallisto)
        {
            bool hasRight = (m_geo->is_onboard(x + 1, y)
                             && points.contains(m_geo->get_point(x + 1, y)));
            bool hasDown = (m_geo->is_onboard(x, y + 1)
                            && points.contains(m_geo->get_point(x, y + 1)));
            Util::paintColorSquareCallisto(painter, m_variant, c, fieldX,
                                           fieldY, m_fieldWidth, hasRight,
                                           hasDown, isOnePiece, alpha,
                                           saturation, flat);
        }
        else
            Util::paintColorSquare(painter, m_variant, c, fieldX, fieldY,
                                   m_fieldWidth, alpha, saturation, flat);
    }
    if (m_isNexos)
        for (auto p : junctions)
        {
            auto x = m_geo->get_x(p);
            auto y = m_geo->get_y(p);
            bool hasLeft = (m_geo->is_onboard(x - 1, y)
                            && points.contains(m_geo->get_point(x - 1, y)));
            bool hasRight = (m_geo->is_onboard(x + 1, y)
                             && points.contains(m_geo->get_point(x + 1, y)));
            bool hasUp = (m_geo->is_onboard(x, y - 1)
                          && points.contains(m_geo->get_point(x, y - 1)));
            bool hasDown = (m_geo->is_onboard(x, y + 1)
                            && points.contains(m_geo->get_point(x, y + 1)));
            Util::paintJunction(painter, m_variant, c, x * m_fieldWidth,
                                y * m_fieldHeight, m_fieldWidth, m_fieldHeight,
                                hasLeft, hasRight, hasUp, hasDown, alpha,
                                saturation);
        }
    painter.restore();
}

void BoardPainter::paintStartingPoints(QPainter& painter, Variant variant,
                                       const Grid<PointState>& pointState,
                                       ColorMap<bool> isFirstPiece)
{
    m_startingPoints.init(variant, *m_geo);
    auto colors = get_colors(variant);
    if (m_isTrigon)
    {
        bool isFirstPieceAny = false;
        for (Color c : colors)
            if (isFirstPiece[c])
            {
                isFirstPieceAny = true;
                break;
            }
        if (! isFirstPieceAny)
            return;
        for (Point p : m_startingPoints.get_starting_points(Color(0)))
        {
            if (! pointState[p].is_empty())
                continue;
            int x = m_geo->get_x(p);
            int y = m_geo->get_y(p);
            qreal fieldX = x * m_fieldWidth;
            qreal fieldY = y * m_fieldHeight;
            bool isUpward = (m_geo->get_point_type(p) == 0);
            Util::paintTriangleStartingPoint(painter, isUpward, fieldX, fieldY,
                                             m_fieldWidth, m_fieldHeight);
        }
    }
    else
    {
        for (Color c : colors)
        {
            if (! isFirstPiece[c])
                continue;
            for (Point p : m_startingPoints.get_starting_points(c))
            {
                if (! pointState[p].is_empty())
                    continue;
                int x = m_geo->get_x(p);
                int y = m_geo->get_y(p);
                qreal fieldX = x * m_fieldWidth;
                qreal fieldY = y * m_fieldHeight;
                if (m_isNexos)
                    Util::paintSegmentStartingPoint(painter, variant, c,
                                                    fieldX, fieldY,
                                                    m_fieldWidth);
                else if (m_isGembloQ)
                    Util::paintGembloQStartingPoint(painter,
                                                    m_geo->get_point_type(p),
                                                    variant, c, fieldX,
                                                    fieldY, m_fieldWidth);
                else
                    Util::paintSquareStartingPoint(painter, variant, c, fieldX,
                                                   fieldY, m_fieldWidth);
            }
        }
    }
}

//-----------------------------------------------------------------------------
