//-----------------------------------------------------------------------------
/** @file libpentobi_gui/Util.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Util.h"

using libpentobi_base::game_variant_classic;
using libpentobi_base::game_variant_classic_2;
using libpentobi_base::game_variant_duo;

//-----------------------------------------------------------------------------

namespace {

const QColor blue(0, 115, 207);

const QColor green(0, 192, 0);

const QColor red(230, 62, 44);

const QColor yellow(235, 205, 35);

const QColor gray(174, 167, 172);

void paintDot(QPainter& painter, QColor color, qreal x, qreal y, qreal width,
              qreal height)
{
    painter.save();
    painter.translate(x, y);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawEllipse(QPointF(0.5 * width, 0.5 * height),
                        0.13 * width, 0.13 * height);
    painter.restore();
}

void paintSquare(QPainter& painter, qreal x, qreal y, qreal size,
                 const QColor& rectColor, const QColor& upLeftColor,
                 const QColor& downRightColor)
{
    painter.save();
    painter.translate(x, y);
    if (! painter.hasClipping()
        || painter.clipRegion().contains(QRect(0, 0, ceil(size), ceil(size))))
    {
        painter.fillRect(QRectF(0, 0, size, size), rectColor);
        qreal border = 0.05 * size;
        const QPointF downRightPolygon[6] =
            {
                QPointF(border, size - border),
                QPointF(size - border, size - border),
                QPointF(size - border, border),
                QPointF(size, 0),
                QPointF(size, size),
                QPointF(0, size)
            };
        painter.setPen(Qt::NoPen);
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
    }
    painter.restore();
}

void paintTriangle(QPainter& painter, bool isUpside, qreal x, qreal y,
                   qreal width, qreal height, const QColor& color,
                   const QColor& upLeftColor, const QColor& downRightColor)
{
    painter.save();
    painter.translate(x, y);
    qreal left = -0.5 * width;
    qreal right = 1.5 * width;
    if (! painter.hasClipping()
        || painter.clipRegion().contains(QRect(floor(left), 0,
                                               ceil(right), ceil(height))))
    {
        if (isUpside)
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
    }
    painter.restore();
}

} //namespace

//-----------------------------------------------------------------------------

string Util::convertSgfValueFromQString(const QString& value,
                                        const string& charset)
{
    QString charsetToLower = QString(charset.c_str()).trimmed().toLower();
    if (charsetToLower == "utf-8" || charsetToLower == "utf8")
        return value.toUtf8().data();
    else if (charsetToLower == "iso-8859-1" || charsetToLower == "latin1"
             || charsetToLower.isEmpty())
        return value.toLatin1().data();
    else
        return value.toAscii().data();
}

QString Util::convertSgfValueToQString(const string& value,
                                       const string& charset)
{
    QString charsetToLower = QString(charset.c_str()).trimmed().toLower();
    if (charsetToLower == "utf-8" || charsetToLower == "utf8")
        return QString::fromUtf8(value.c_str());
    else if (charsetToLower == "iso-8859-1" || charsetToLower == "latin1"
             || charsetToLower.isEmpty())
        return QString::fromLatin1(value.c_str());
    else
        return QString::fromAscii(value.c_str());
}

QColor Util::getLabelColor(GameVariant gameVariant, PointState s)
{
    if (s.is_empty())
        return Qt::black;
    Color c = s.to_color();
    QColor paintColor = getPaintColor(gameVariant, c);
    if (paintColor == yellow || paintColor == green)
        return Qt::black;
    else
        return Qt::white;
}

QColor Util::getPaintColor(GameVariant gameVariant, Color c)
{
    if (gameVariant == game_variant_duo)
        return c == Color(0) ? blue : green;
    else
    {
        if (c == Color(0))
            return blue;
        if (c == Color(1))
            return yellow;
        if (c == Color(2))
            return red;
        LIBBOARDGAME_ASSERT(c == Color(3));
        return green;
    }
}

QColor Util::getPaintColorEmpty()
{
    return gray;
}

void Util::paintColorSquare(QPainter& painter, GameVariant gameVariant,
                            Color c, qreal x, qreal y, qreal size)
{
    QColor rectColor = getPaintColor(gameVariant, c);
    QColor upLeftColor = rectColor.lighter(130);
    QColor downRightColor = rectColor.darker(160);
    paintSquare(painter, x, y, size, rectColor, upLeftColor, downRightColor);
}

void Util::paintColorTriangle(QPainter& painter, GameVariant gameVariant,
                              Color c, bool isUpside, qreal x, qreal y,
                              qreal width, qreal height)
{
    QColor color = getPaintColor(gameVariant, c);
    QColor upLeftColor = color.lighter(130);
    QColor downRightColor = color.darker(160);
    paintTriangle(painter, isUpside, x, y, width, height, color, upLeftColor,
                  downRightColor);
}

void Util::paintEmptySquare(QPainter& painter, qreal x, qreal y, qreal size)
{
    paintSquare(painter, x, y, size, gray, gray.darker(130), gray.lighter(115));
}

void Util::paintEmptyTriangle(QPainter& painter, bool isUpside, qreal x,
                              qreal y, qreal width, qreal height)
{
    paintTriangle(painter, isUpside, x, y, width, height, gray,
                  gray.darker(130), gray.lighter(115));
}

void Util::paintEmptyTriangleStartingPoint(QPainter& painter, bool isUpside,
                                           qreal x, qreal y, qreal width,
                                           qreal height)
{
    paintEmptyTriangle(painter, isUpside, x, y, width, height);
    if (isUpside)
        y += 0.333 * height;
    height = 0.666 * height;
    paintDot(painter, gray.darker(130), x, y, width, height);
}

void Util::paintEmptySquareStartingPoint(QPainter& painter,
                                         GameVariant gameVariant, Color c,
                                         qreal x, qreal y, qreal size)
{
    paintEmptySquare(painter, x, y, size);
    paintDot(painter, getPaintColor(gameVariant, c), x, y, size, size);
}

//-----------------------------------------------------------------------------
