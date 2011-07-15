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

const QColor yellow(245, 215, 37);

const QColor gray(200, 191, 197);

void paintDot(QPainter& painter, GameVariant gameVariant, Color c, int x,
              int y, int size)
{
    QColor color = Util::getPaintColor(gameVariant, c);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.save();
    painter.translate(x, y);
    painter.setPen(color);
    painter.setBrush(color);
    painter.drawEllipse(QPoint(size / 2, size / 2), size / 8, size / 8);
    painter.restore();
    painter.setRenderHint(QPainter::Antialiasing, false);
}

void paintSquare(QPainter& painter, int x, int y, int size,
                 const QColor& rectColor, const QColor& upLeftColor,
                 const QColor& downRightColor)
{
    painter.save();
    painter.translate(x, y);
    if (! painter.hasClipping()
        || painter.clipRegion().contains(QRect(0, 0, size, size)))
    {
        int border;
        if (size < 5)
            border = 0;
        else if (size < 12)
            border = 1;
        else
            border = size / 12;
        const QPoint upLeftPolygon[6] =
            {
                QPoint(0, 0),
                QPoint(size - 1, 0),
                QPoint(size - border, border - 1),
                QPoint(border - 1, border - 1),
                QPoint(border - 1, size - border),
                QPoint(0, size - 1)
            };
        const QPoint downRightPolygon[6] =
            {
                QPoint(0, size - 1),
                QPoint(border - 1, size - border),
                QPoint(size - border, size - border),
                QPoint(size - border, border - 1),
                QPoint(size - 1, 0),
                QPoint(size - 1, size - 1)
            };
        painter.fillRect(0, 0, size, size, rectColor);
        if (border > 0)
        {
            painter.setPen(downRightColor);
            painter.setBrush(downRightColor);
            painter.drawConvexPolygon(downRightPolygon, 6);
            painter.setPen(upLeftColor);
            painter.setBrush(upLeftColor);
            painter.drawConvexPolygon(upLeftPolygon, 6);
        }
    }
    painter.restore();
}

} //namespace

//-----------------------------------------------------------------------------

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
                            Color c, int x, int y, int size)
{
    QColor rectColor = getPaintColor(gameVariant, c);
    QColor upLeftColor = rectColor.lighter();
    QColor downRightColor = rectColor.darker();
    paintSquare(painter, x, y, size, rectColor, upLeftColor, downRightColor);
}

void Util::paintEmptySquare(QPainter& painter, int x, int y, int size)
{
    paintSquare(painter, x, y, size, gray, gray.darker(110), gray.lighter(110));
}

void Util::paintEmptySquareStartingPoint(QPainter& painter,
                                         GameVariant gameVariant, Color c,
                                         int x, int y, int size)
{
    paintEmptySquare(painter, x, y, size);
    paintDot(painter, gameVariant, c, x, y, size);
}

//-----------------------------------------------------------------------------
