//-----------------------------------------------------------------------------
/** @file libpentobi_gui/Util.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Util.h"

#include <QCoreApplication>

//-----------------------------------------------------------------------------

namespace {

const QColor blue(0, 115, 207);

const QColor green(0, 192, 0);

const QColor red(230, 62, 44);

const QColor yellow(235, 205, 35);

const QColor purple(161, 44, 207);

const QColor orange(240, 173, 26);

const QColor gray(174, 167, 172);

void setAlphaSaturation(QColor& c, qreal alpha, qreal saturation)
{
    if (saturation != 1)
        c.setHsv(c.hue(), static_cast<int>(saturation * c.saturation()),
                 c.value());
    if (alpha != 1)
        c.setAlphaF(alpha);
}

void paintDot(QPainter& painter, const QColor& color, qreal x, qreal y,
              qreal width, qreal height, qreal size)
{
    painter.save();
    painter.translate(x, y);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawEllipse(QPointF(0.5 * width, 0.5 * height), size, size);
    painter.restore();
}

void paintGembloQ(QPainter& painter, unsigned pointType, qreal x, qreal y,
                  qreal width, const QColor& color, const QColor& upColor,
                  const QColor& downColor, bool flat)
{
    painter.save();
    painter.translate(x, y);
    qreal border = 0.2 * width;
    if (flat)
        painter.setPen(Qt::NoPen);
    else
        painter.setPen(color);
    painter.setBrush(color);
    switch (pointType)
    {
    case 0:
    {
        const QPointF trianglePolygon[3] =
        {
            QPointF(0, 0),
            QPointF(2 * width - border, 0),
            QPointF(0, 2 * width - border)
        };
        painter.drawPolygon(trianglePolygon, 3);
        const QPointF borderPolygon[4] =
        {
            QPointF(2 * width - border, 0),
            QPointF(2 * width, 0),
            QPointF(0, 2 * width),
            QPointF(0, 2 * width - border)
        };
        painter.setPen(downColor);
        painter.setBrush(downColor);
        painter.drawPolygon(borderPolygon, 4);
        break;
    }
    case 1:
    {
        const QPointF trianglePolygon[3] =
        {
            QPointF(width, border),
            QPointF(width, 2 * width),
            QPointF(-width + border, 2 * width)
        };
        painter.drawPolygon(trianglePolygon, 3);
        const QPointF borderPolygon[4] =
        {
            QPointF(width, 0),
            QPointF(width, border),
            QPointF(-width + border, 2 * width),
            QPointF(-width, 2 * width)
        };
        painter.setPen(upColor);
        painter.setBrush(upColor);
        painter.drawPolygon(borderPolygon, 4);
        break;
    }
    case 2:
    {
        const QPointF trianglePolygon[3] =
        {
            QPointF(0, border),
            QPointF(2 * width - border, 2 * width),
            QPointF(0, 2 * width)
        };
        painter.drawPolygon(trianglePolygon, 3);
        const QPointF borderPolygon[4] =
        {
            QPointF(0, 0),
            QPointF(2 * width, 2 * width),
            QPointF(2 * width - border, 2 * width),
            QPointF(0, border)
        };
        painter.setPen(upColor);
        painter.setBrush(upColor);
        painter.drawPolygon(borderPolygon, 4);
        break;
    }
    case 3:
    {
        const QPointF trianglePolygon[3] =
        {
            QPointF(-width + border, 0),
            QPointF(width, 0),
            QPointF(width, 2 * width - border)
        };
        painter.drawPolygon(trianglePolygon, 3);
        const QPointF borderPolygon[4] =
        {
            QPointF(-width, 0),
            QPointF(-width + border, 0),
            QPointF(width, 2 * width - border),
            QPointF(width, 2 * width)
        };
        painter.setPen(downColor);
        painter.setBrush(downColor);
        painter.drawPolygon(borderPolygon, 4);
        break;
    }
    }
    painter.restore();
}


void paintSquare(QPainter& painter, qreal x, qreal y, qreal width,
                 qreal height, const QColor& rectColor,
                 const QColor& upLeftColor, const QColor& downRightColor,
                 bool onlyBorder = false)
{
    painter.save();
    painter.translate(x, y);
    if (! onlyBorder)
        painter.fillRect(QRectF(0, 0, width, height), rectColor);
    qreal border = 0.05 * max(width, height);
    const QPointF downRightPolygon[6] =
        {
            QPointF(border, height - border),
            QPointF(width - border, height - border),
            QPointF(width - border, border),
            QPointF(width, 0),
            QPointF(width, height),
            QPointF(0, height)
        };
    painter.setPen(Qt::NoPen);
    painter.setBrush(downRightColor);
    painter.drawPolygon(downRightPolygon, 6);
    const QPointF upLeftPolygon[6] =
        {
            QPointF(0, 0),
            QPointF(width, 0),
            QPointF(width - border, border),
            QPointF(border, border),
            QPointF(border, height - border),
            QPointF(0, height)
        };
    painter.setBrush(upLeftColor);
    painter.drawPolygon(upLeftPolygon, 6);
    painter.restore();
}

void paintTriangle(QPainter& painter, bool isUpward, qreal x, qreal y,
                   qreal width, qreal height, const QColor& color,
                   const QColor& upLeftColor, const QColor& downRightColor)
{
    painter.save();
    painter.translate(x, y);
    qreal left = -0.5 * width;
    qreal right = 1.5 * width;
    if (isUpward)
    {
        const QPointF polygon[3] =
            {
                QPointF(left, height),
                QPointF(right, height),
                QPointF(0.5 * width, 0)
            };
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawConvexPolygon(polygon, 3);
        qreal border = 0.08 * width;
        const QPointF downRightPolygon[6] =
            {
                QPointF(left, height),
                QPointF(right, height),
                QPointF(0.5 * width, 0),
                QPointF(0.5 * width, 2 * border),
                QPointF(right - 1.732 * border, height - border),
                QPointF(left + 1.732 * border, height - border)
            };
        painter.setBrush(downRightColor);
        painter.drawPolygon(downRightPolygon, 6);
        const QPointF upLeftPolygon[4] =
            {
                QPointF(0.5 * width, 0),
                QPointF(0.5 * width, 2 * border),
                QPointF(left + 1.732 * border, height - border),
                QPointF(left, height),
            };
        painter.setBrush(upLeftColor);
        painter.drawPolygon(upLeftPolygon, 4);
    }
    else
    {
        const QPointF polygon[3] =
            {
                QPointF(left, 0),
                QPointF(right, 0),
                QPointF(0.5 * width, height)
            };
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawConvexPolygon(polygon, 3);
        qreal border = 0.05 * width;
        const QPointF downRightPolygon[4] =
            {
                QPointF(0.5 * width, height),
                QPointF(0.5 * width, height - 2 * border),
                QPointF(right - 1.732 * border, border),
                QPointF(right, 0)
            };
        painter.setBrush(downRightColor);
        painter.drawPolygon(downRightPolygon, 4);
        const QPointF upLeftPolygon[6] =
            {
                QPointF(right, 0),
                QPointF(right - 1.732 * border, border),
                QPointF(left + 1.732 * border, border),
                QPointF(0.5 * width, height - 2 * border),
                QPointF(0.5 * width, height),
                QPointF(left, 0)
            };
        painter.setBrush(upLeftColor);
        painter.drawPolygon(upLeftPolygon, 6);
    }
    painter.restore();
}

void paintSquareFrame(QPainter& painter, qreal x, qreal y, qreal size,
                      const QColor& rectColor, const QColor& upLeftColor,
                      const QColor& downRightColor)
{
    painter.save();
    painter.translate(x, y);
    painter.setPen(Qt::NoPen);
    qreal border = 0.05 * size;
    qreal frameSize = 0.17 * size;
    painter.fillRect(QRectF(0, 0, size, frameSize), rectColor);
    painter.fillRect(QRectF(0, size - frameSize, size, frameSize), rectColor);
    painter.fillRect(QRectF(0, 0, frameSize, size), rectColor);
    painter.fillRect(QRectF(size - frameSize, 0, frameSize, size), rectColor);
    const QPointF downRightPolygon[6] =
        {
            QPointF(border, size - border),
            QPointF(size - border, size - border),
            QPointF(size - border, border),
            QPointF(size, 0),
            QPointF(size, size),
            QPointF(0, size)
        };
    painter.setBrush(downRightColor);
    painter.drawPolygon(downRightPolygon, 6);
    const QPointF upLeftPolygon[6] =
        {
            QPointF(0, 0),
            QPointF(size, 0),
            QPointF(size - border, border),
            QPointF(border, border),
            QPointF(border, size - border),
            QPointF(0, size)
        };
    painter.setBrush(upLeftColor);
    painter.drawPolygon(upLeftPolygon, 6);
    painter.restore();
}

void paintColorSquareFrame(QPainter& painter, Variant variant, Color c,
                           qreal x, qreal y, qreal size, qreal alpha,
                           qreal saturation, bool flat)
{
    auto color = Util::getPaintColor(variant, c);
    QColor upLeftColor;
    QColor downRightColor;
    if (flat)
    {
        upLeftColor = color;
        downRightColor = color;
    }
    else
    {
        upLeftColor = color.lighter(130);
        downRightColor = color.darker(160);
    }
    setAlphaSaturation(color, alpha, saturation);
    setAlphaSaturation(upLeftColor, alpha, saturation);
    setAlphaSaturation(downRightColor, alpha, saturation);
    paintSquareFrame(painter, x, y, size, color, upLeftColor, downRightColor);
}

} //namespace

//-----------------------------------------------------------------------------

QColor Util::getLabelColor(Variant variant, PointState s)
{
    if (s.is_empty())
        return Qt::black;
    Color c = s.to_color();
    QColor paintColor = getPaintColor(variant, c);
    if (paintColor == yellow || paintColor == green)
        return Qt::black;
    return Qt::white;
}

QColor Util::getMarkColor(Variant variant, PointState s)
{
    if (s.is_empty())
        return Qt::white;
    Color c = s.to_color();
    QColor paintColor = getPaintColor(variant, c);
    if (paintColor == yellow || paintColor == green)
        return QColor(51, 51, 51);
    return Qt::white;
}

QColor Util::getPaintColor(Variant variant, Color c)
{
    if (variant == Variant::duo)
        return c == Color(0) ? purple : orange;
    if (variant == Variant::junior)
        return c == Color(0) ? green : orange;
    if (get_nu_colors(variant) == 2)
        return c == Color(0) ? blue : green;
    if (c == Color(0))
        return blue;
    if (c == Color(1))
        return yellow;
    if (c == Color(2))
        return red;
    LIBBOARDGAME_ASSERT(c == Color(3));
    return green;
}

QString Util::getPlayerString(Variant variant, Color c)
{
    auto i = c.to_int();
    if (variant == Variant::duo)
        return i == 0 ? qApp->translate("Util", "Purple")
                      : qApp->translate("Util", "Orange");
    if (variant == Variant::junior)
        return i == 0 ? qApp->translate("Util", "Green")
                      : qApp->translate("Util", "Orange");
    if (get_nu_colors(variant) == 2)
        return i == 0 ? qApp->translate("Util", "Blue")
                      : qApp->translate("Util", "Green");
    if (get_nu_players(variant) == 2)
        return i == 0 || i == 2 ? qApp->translate("Util", "Blue/Red")
                                : qApp->translate("Util", "Yellow/Green");
    if (i == 0)
        return qApp->translate("Util", "Blue");
    if (i == 1)
        return qApp->translate("Util", "Yellow");
    if (i == 2)
        return qApp->translate("Util", "Red");
    return qApp->translate("Util", "Green");
}

void Util::paintColorSegment(QPainter& painter, Variant variant, Color c,
                             bool isHorizontal, qreal x, qreal y, qreal size,
                             qreal alpha, qreal saturation, bool flat)
{
    auto color = getPaintColor(variant, c);
    QColor upLeftColor;
    QColor downRightColor;
    if (flat)
    {
        upLeftColor = color;
        downRightColor = color;
    }
    else
    {
        upLeftColor = color.lighter(130);
        downRightColor = color.darker(160);
    }
    setAlphaSaturation(color, alpha, saturation);
    setAlphaSaturation(upLeftColor, alpha, saturation);
    setAlphaSaturation(downRightColor, alpha, saturation);
    if (isHorizontal)
        paintSquare(painter, x - size / 4, y + size / 4, 1.5 * size, size / 2,
                    color, upLeftColor, downRightColor);
    else
        paintSquare(painter, x + size / 4, y - size / 4, size / 2, 1.5 * size,
                    color, upLeftColor, downRightColor);
}

void Util::paintColorGembloQ(QPainter& painter, Variant variant, Color c,
                             unsigned pointType, qreal x, qreal y, qreal width,
                             qreal alpha, qreal saturation, bool flat)
{
    auto color = getPaintColor(variant, c);
    QColor upColor;
    QColor downColor;
    if (flat)
    {
        upColor = color;
        downColor = color;
    }
    else
    {
        upColor = color.lighter(125);
        downColor = color.darker(130);
    }
    setAlphaSaturation(color, alpha, saturation);
    setAlphaSaturation(upColor, alpha, saturation);
    setAlphaSaturation(downColor, alpha, saturation);
    paintGembloQ(painter, pointType, x, y, width, color, upColor, downColor,
                 flat);
}

void Util::paintColorSquare(QPainter& painter, Variant variant, Color c,
                            qreal x, qreal y, qreal size, qreal alpha,
                            qreal saturation, bool flat)
{
    auto color = getPaintColor(variant, c);
    QColor upLeftColor;
    QColor downRightColor;
    if (flat)
    {
        upLeftColor = color;
        downRightColor = color;
    }
    else
    {
        upLeftColor = color.lighter(130);
        downRightColor = color.darker(160);
    }
    setAlphaSaturation(color, alpha, saturation);
    setAlphaSaturation(upLeftColor, alpha, saturation);
    setAlphaSaturation(downRightColor, alpha, saturation);
    paintSquare(painter, x, y, size, size, color, upLeftColor, downRightColor);
}

void Util::paintColorSquareCallisto(QPainter& painter, Variant variant,
                                    Color c, qreal x, qreal y, qreal size,
                                    bool hasRight, bool hasDown,
                                    bool isOnePiece, qreal alpha,
                                    qreal saturation, bool flat)
{
    auto color = getPaintColor(variant, c);
    setAlphaSaturation(color, alpha, saturation);
    if (hasRight)
        painter.fillRect(QRectF(x + 0.96 * size, y + 0.07 * size,
                                0.08 * size, 0.86 * size), color);
    if (hasDown)
        painter.fillRect(QRectF(x + 0.07 * size, y + 0.96 * size,
                                0.86 * size, 0.08 * size), color);
    if (isOnePiece)
        paintColorSquareFrame(painter, variant, c, x + 0.04 * size,
                              y + 0.04 * size, 0.92 * size, alpha, saturation,
                              flat);
    else
        paintColorSquare(painter, variant, c, x + 0.04 * size, y + 0.04 * size,
                         0.92 * size, alpha, saturation, flat);
}

void Util::paintColorTriangle(QPainter& painter, Variant variant,
                              Color c, bool isUpward, qreal x, qreal y,
                              qreal width, qreal height, qreal alpha,
                              qreal saturation, bool flat)
{
    auto color = getPaintColor(variant, c);
    QColor upLeftColor;
    QColor downRightColor;
    if (flat)
    {
        upLeftColor = color;
        downRightColor = color;
    }
    else
    {
        upLeftColor = color.lighter(130);
        downRightColor = color.darker(160);
    }
    setAlphaSaturation(color, alpha, saturation);
    setAlphaSaturation(upLeftColor, alpha, saturation);
    setAlphaSaturation(downRightColor, alpha, saturation);
    paintTriangle(painter, isUpward, x, y, width, height, color, upLeftColor,
                  downRightColor);
}

void Util::paintEmptyGembloQ(QPainter& painter, unsigned pointType, qreal x,
                             qreal y, qreal width)
{
    paintGembloQ(painter, pointType, x, y, width, gray, gray.darker(130),
                 gray.lighter(115), false);
}

void Util::paintEmptyJunction(QPainter& painter, qreal x, qreal y, qreal size)
{
    painter.fillRect(QRectF(x + 0.25 * size, y + 0.25 * size,
                            0.5 * size, 0.5 * size),
                     gray);
}

void Util::paintEmptySegment(QPainter& painter, bool isHorizontal, qreal x,
                             qreal y, qreal size)
{
    if (isHorizontal)
        paintSquare(painter, x - size / 4, y + size / 4, 1.5 * size, size / 2,
                    gray, gray.darker(130), gray.lighter(115));
    else
        paintSquare(painter, x + size / 4, y - size / 4, size / 2, 1.5 * size,
                    gray, gray.darker(130), gray.lighter(115));
}

void Util::paintEmptySquare(QPainter& painter, qreal x, qreal y, qreal size)
{
    paintSquare(painter, x, y, size, size, gray, gray.darker(130),
                gray.lighter(115));
}

void Util::paintEmptySquareCallisto(QPainter& painter, qreal x, qreal y,
                                    qreal size)
{
    painter.fillRect(QRectF(x, y, size, size), gray);
    paintSquare(painter, x + 0.04 * size, y + 0.04 * size, 0.92 * size,
                0.92 * size, gray, gray.darker(130), gray.lighter(115), true);
}

void Util::paintEmptySquareCallistoCenter(QPainter& painter, qreal x, qreal y,
                                          qreal size)
{
    painter.fillRect(QRectF(x, y, size, size), gray);
    paintSquare(painter, x + 0.05 * size, y + 0.05 * size, 0.9 * size,
                0.9 * size, gray.darker(120), gray.darker(150),
                gray.lighter(95), false);
}

void Util::paintGembloQStartingPoint(QPainter& painter, unsigned pointType,
                                     Variant variant, Color c, qreal x,
                                     qreal y, qreal width)
{
    switch (pointType)
    {
    case 0:
        x -= width;
        y -= width;
        break;
    case 1:
        y += width;
        break;
    case 2:
        x -= width;
        y += width;
        break;
    case 3:
        y -= width;
        break;
    }
    paintDot(painter, getPaintColor(variant, c), x, y,
             2 * width, 2 * width, 0.4 * width);
}

void Util::paintEmptyTriangle(QPainter& painter, bool isUpward, qreal x,
                              qreal y, qreal width, qreal height)
{
    paintTriangle(painter, isUpward, x, y, width, height, gray,
                  gray.darker(130), gray.lighter(115));
}

void Util::paintJunction(QPainter& painter, Variant variant, Color c, qreal x,
                         qreal y, qreal width, qreal height, bool hasLeft,
                         bool hasRight, bool hasUp, bool hasDown, qreal alpha,
                         qreal saturation)
{
    auto color = getPaintColor(variant, c);
    setAlphaSaturation(color, alpha, saturation);
    painter.save();
    painter.translate(x + 0.25 * width, y + 0.25 * height);
    width *= 0.5;
    height *= 0.5;
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    if (hasLeft && hasUp && ! hasRight && ! hasDown)
    {
        const QPointF polygon[3] = { QPointF(0, 0),
                                     QPointF(0.75 * width, 0),
                                     QPointF(0, 0.75 * height) };
        painter.drawPolygon(polygon, 3);
    }
    else if (hasRight && hasUp && ! hasLeft && ! hasDown)
    {
        const QPointF polygon[3] = { QPointF(0.25 * width, 0),
                                     QPointF(width, 0),
                                     QPointF(width, 0.75 * height) };
        painter.drawPolygon(polygon, 3);
    }
    else if (hasLeft && hasDown && ! hasRight && ! hasUp)
    {
        const QPointF polygon[3] = { QPointF(0, 0.25 * height),
                                     QPointF(0, height),
                                     QPointF(0.75 * width, height) };
        painter.drawPolygon(polygon, 3);
    }
    else if (hasRight && hasDown && ! hasLeft && ! hasUp)
    {
        const QPointF polygon[3] = { QPointF(0.25 * width, height),
                                     QPointF(width, 0.25 * height),
                                     QPointF(width, height) };
        painter.drawPolygon(polygon, 3);
    }
    else
    {
        if (hasUp && (hasDown || hasLeft || hasRight))
            painter.fillRect(QRectF(0.25 * width, 0, 0.5 * width, 0.5 * height), color);
        if (hasDown && (hasUp || hasLeft || hasRight))
            painter.fillRect(QRectF(0.25 * width, 0.5 * height, 0.5 * width, height), color);
        if (hasLeft && (hasUp || hasDown || hasRight))
            painter.fillRect(QRectF(0, 0.25 * height, 0.5 * width, 0.5 * height), color);
        if (hasRight && (hasUp || hasDown || hasLeft))
            painter.fillRect(QRectF(0.5 * width , 0.25 * height, width, 0.5 * height), color);
    }
    painter.restore();
}

void Util::paintSegmentStartingPoint(QPainter& painter, Variant variant,
                                          Color c, qreal x, qreal y,
                                     qreal size)
{
    paintDot(painter, getPaintColor(variant, c), x, y, size, size,
             0.15 * size);
}

void Util::paintSquareStartingPoint(QPainter& painter, Variant variant,
                                    Color c, qreal x, qreal y, qreal size)
{
    paintDot(painter, getPaintColor(variant, c), x, y, size, size,
             0.13 * size);
}

void Util::paintTriangleStartingPoint(QPainter& painter, bool isUpward,
                                      qreal x, qreal y, qreal width,
                                      qreal height)
{
    if (isUpward)
        y += 0.333 * height;
    height = 0.666 * height;
    paintDot(painter, gray.darker(130), x, y, width, height, 0.17 * width);
}

//-----------------------------------------------------------------------------
