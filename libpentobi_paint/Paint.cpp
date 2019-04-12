//-----------------------------------------------------------------------------
/** @file libpentobi_paint/Paint.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Paint.h"

#include <QPainter>
#include "libpentobi_base/CallistoGeometry.h"
#include "libpentobi_base/ColorMap.h"

using namespace std;
using libboardgame_base::ArrayList;
using libpentobi_base::CallistoGeometry;
using libpentobi_base::Color;
using libpentobi_base::ColorMap;
using libpentobi_base::Geometry;
using libpentobi_base::GeometryType;
using libpentobi_base::Point;

namespace libpentobi_paint {

//-----------------------------------------------------------------------------

namespace {

void paintQuarterSquareBase(QPainter& painter, qreal x, qreal y, qreal width,
                            qreal height, const QColor& base)
{
    const QPointF polygon[3] =
    {
        {x, y},
        {x + width, y},
        {x, y + height}
    };
    painter.setPen(Qt::NoPen);
    painter.setBrush(base);
    painter.drawConvexPolygon(polygon, 3);
}

void paintQuarterSquareFrame(QPainter& painter, qreal x, qreal y, qreal width,
                             qreal height, const QColor& light)
{
    const QPointF polygon[4] =
    {
        {x, y + height},
        {x, y + 0.9 * height},
        {x + 0.9 * width, y},
        {x + width, y}
    };
    painter.setPen(Qt::NoPen);
    painter.setBrush(light);
    painter.drawConvexPolygon(polygon, 4);
}

void paintSquareFrame(QPainter& painter, qreal x, qreal y, qreal width,
                      qreal height, const QColor& light,
                      const QColor& dark)
{
    painter.save();
    painter.translate(x, y);
    qreal border = 0.05 * max(width, height);
    const QPointF down[4] =
        {
            {border, height - border},
            {width - border, height - border},
            {width, height},
            {0, height}
        };
    const QPointF right[4] =
        {
            {width - border, height - border},
            {width - border, border},
            {width, 0},
            {width, height}
        };
    const QPointF up[4] =
        {
            {0, 0},
            {width, 0},
            {width - border, border},
            {border, border}
        };
    const QPointF left[4] =
        {
            {0, 0},
            {border, border},
            {border, height - border},
            {0, height}
        };
    painter.setPen(Qt::NoPen);
    painter.setBrush(dark);
    painter.drawConvexPolygon(down, 4);
    painter.drawConvexPolygon(right, 4);
    painter.setBrush(light);
    painter.drawConvexPolygon(up, 4);
    painter.drawConvexPolygon(left, 4);
    painter.restore();
}

void paintTriangleDownFrame(QPainter& painter, qreal x, qreal y, qreal width,
                            qreal height, const QColor& light,
                            const QColor& dark)
{
    painter.save();
    painter.translate(x, y);
    auto border = 0.05 * height;
    const QPointF left[4] =
    {
        {0.5 * width, height},
        {0.5 * width, height - 2 * border},
        {width - 1.732 * border, border},
        {width, 0}
    };
    const QPointF right[4] =
    {
        {0.5 * width, height},
        {0.5 * width, height - 2 * border},
        {1.732 * border, border},
        {0, 0}
    };
    const QPointF up[4] =
    {
        {width, 0},
        {width - 1.732 * border, border},
        {1.732 * border, border},
        {0, 0}
    };
    painter.setPen(Qt::NoPen);
    painter.setBrush(dark);
    painter.drawConvexPolygon(left, 4);
    painter.drawConvexPolygon(right, 4);
    painter.setBrush(light);
    painter.drawConvexPolygon(up, 4);
    painter.restore();
}

void paintTriangleUpFrame(QPainter& painter, qreal x, qreal y, qreal width,
                          qreal height, const QColor& light,
                          const QColor& dark)
{
    painter.save();
    painter.translate(x, y);
    auto border = 0.05 * height;
    const QPointF down[4] =
    {
        {0, height},
        {width, height},
        {width - 1.732 * border, height - border},
        {1.732 * border, height - border}
    };
    const QPointF left[4] =
    {
        {0.5 * width, 0},
        {0.5 * width, 2 * border},
        {1.732 * border, height - border},
        {0, height}
    };
    const QPointF right[4] =
    {
        {0.5 * width, 0},
        {0.5 * width, 2 * border},
        {width - 1.732 * border, height - border},
        {width, height}
    };
    painter.setPen(Qt::NoPen);
    painter.setBrush(dark);
    painter.drawConvexPolygon(down, 4);
    painter.setBrush(light);
    painter.drawConvexPolygon(left, 4);
    painter.drawConvexPolygon(right, 4);
    painter.restore();
}

void paintBoardCallisto(QPainter& painter, qreal width, qreal height,
                        const Geometry& geo, unsigned nuColors,
                        const QColor& base, const QColor& light,
                        const QColor& dark, const QColor& centerBase,
                        const QColor& centerLight, const QColor& centerDark)
{
    auto gridWidth = width / geo.get_width();
    auto gridHeight = height / geo.get_height();
    for (auto p : geo)
    {
        auto x = geo.get_x(p);
        auto y = geo.get_y(p);
        painter.save();
        painter.translate(x * gridWidth, y * gridHeight);
        painter.scale(gridWidth, gridHeight);
        painter.fillRect(QRectF(0, 0, 1, 1), base);
        painter.save();
        painter.translate(0.025, 0.025);
        painter.scale(0.95, 0.95);
        if (CallistoGeometry::is_center_section(x, y, nuColors))
        {
            painter.fillRect(QRectF(0, 0, 1, 1), centerBase);
            paintSquareFrame(painter, 0, 0, 1, 1, centerDark, centerLight);
        }
        else
            paintSquareFrame(painter, 0, 0, 1, 1, dark, light);
        painter.restore();
        painter.restore();
    }
}

void paintBoardClassic(QPainter& painter, qreal width, qreal height,
                       const Geometry& geo, const QColor& base,
                       const QColor& light, const QColor& dark)
{
    painter.fillRect(QRectF(0, 0, width, height), base);
    auto gridWidth = width / geo.get_width();
    auto gridHeight = height / geo.get_height();
    for (unsigned x = 0; x < geo.get_width(); ++x)
        for (unsigned y = 0; y < geo.get_height(); ++y)
            paintSquareFrame(painter, x * gridWidth, y * gridHeight, gridWidth,
                             gridHeight, dark, light);
}

void paintBoardNexos(QPainter& painter, qreal width, qreal height,
                     const Geometry& geo, const QColor& base,
                     const QColor& light, const QColor& dark)
{
    painter.fillRect(QRectF(0, 0, width, height), base);
    auto gridWidth = width / (geo.get_width() - 0.5);
    auto gridHeight = height / (geo.get_height() - 0.5);
    for (unsigned x = 1; x < geo.get_width(); x += 2)
        for (unsigned y = 0; y < geo.get_height(); y += 2)
            paintSquareFrame(painter, x * gridWidth - 0.5 * gridWidth,
                             y * gridHeight, 1.5 * gridWidth, 0.5 * gridHeight,
                             dark, light);
    for (unsigned x = 0; x < geo.get_width(); x += 2)
        for (unsigned y = 1; y < geo.get_height(); y += 2)
            paintSquareFrame(painter, x * gridWidth,
                             y * gridHeight - 0.5 * gridHeight,
                             0.5 * gridWidth, 1.5 * gridHeight, dark, light);
}

void paintBoardGembloQ(QPainter& painter, qreal width, qreal height,
                      const Geometry& geo, const QColor& base,
                      const QColor& light, const QColor& dark)
{
    auto gridWidth = width / geo.get_width();
    auto gridHeight = height / geo.get_height();
    qreal distX, distY;
    switch (geo.get_height())
    {
    case 22:
    case 26:
        distX = 14 * gridWidth;
        distY = 7 * gridHeight;
        break;
    default:
        LIBBOARDGAME_ASSERT(geo.get_height() == 28);
        distX = 2 * gridWidth;
        distY = gridHeight;
        break;
    }
    const QPointF board[8] =
    {
        {distX, 0},
        {width - distX, 0},
        {width, distY},
        {width, height - distY},
        {width - distX, height},
        {distX, height},
        {0, height - distY},
        {0, distY}
    };
    painter.setPen(Qt::NoPen);
    painter.setBrush(base);
    painter.drawConvexPolygon(board, 8);
    for (auto p : geo)
    {
        painter.save();
        painter.translate(QPointF(geo.get_x(p) * gridWidth,
                                  geo.get_y(p) * gridHeight));
        QColor border;
        switch (geo.get_point_type(p))
        {
        case 0:
            border = light;
            break;
        case 1:
            border = dark;
            painter.rotate(180);
            painter.translate(-gridWidth, -gridHeight);
            break;
        case 2:
            border = dark;
            painter.rotate(270);
            painter.translate(-gridHeight, 0);
            break;
        case 3:
            border = light;
            painter.rotate(90);
            painter.translate(0, -gridWidth);
            break;
        }
        paintQuarterSquareFrame(painter, 0, 0, 2 * gridWidth, gridHeight,
                                border);
        painter.restore();
    }
}

void paintBoardTrigon(QPainter& painter, qreal width, qreal height,
                      const Geometry& geo, const QColor& base,
                      const QColor& light, const QColor& dark)
{
    auto gridWidth = width / (geo.get_width() + 1);
    auto gridHeight = height / geo.get_height();
    auto dist = (geo.get_width() + 1 - geo.get_height()) * gridWidth/ 2;
    const QPointF board[6] =
    {
        {dist, 0},
        {width - dist, 0},
        {width, height / 2},
        {width - dist, height},
        {dist, height},
        {0, height / 2}
    };
    painter.setPen(Qt::NoPen);
    painter.setBrush(base);
    painter.drawConvexPolygon(board, 6);
    for (auto p : geo)
        if (geo.get_point_type(p) == 0)
            paintTriangleUpFrame(painter, geo.get_x(p) * gridWidth - 0.5,
                                 geo.get_y(p) * gridHeight, 2 * gridWidth,
                                 gridHeight, dark, light);
        else
            paintTriangleDownFrame(painter, geo.get_x(p) * gridWidth - 0.5,
                                   geo.get_y(p) * gridHeight, 2 * gridWidth,
                                   gridHeight, dark, light);
}

void paintPiecesCallisto(
        QPainter& painter, qreal width, qreal height, const Geometry& geo,
        const Grid<PointState>& pointState, const Grid<unsigned>& pieceId,
        const ColorMap<QColor>& base, const ColorMap<QColor>& light,
        const ColorMap<QColor>& dark)
{
    auto gridWidth = width / geo.get_width();
    auto gridHeight = height / geo.get_height();
    for (auto p : geo)
    {
        if (pointState[p].is_empty())
            continue;
        auto c = pointState[p].to_color();
        auto x = geo.get_x(p);
        auto y = geo.get_y(p);
        bool hasLeft =
                (geo.is_onboard(x - 1, y)
                 && pieceId[p] == pieceId[geo.get_point(x - 1, y)]);
        bool hasRight =
                (geo.is_onboard(x + 1, y)
                 && pieceId[p] == pieceId[geo.get_point(x + 1, y)]);
        bool hasUp =
                (geo.is_onboard(x, y - 1)
                 && pieceId[p] == pieceId[geo.get_point(x, y - 1)]);
        bool hasDown =
                (geo.is_onboard(x, y + 1)
                 && pieceId[p] == pieceId[geo.get_point(x, y + 1)]);
        bool hasRightDown =
                (geo.is_onboard(x + 1, y + 1)
                 && pieceId[p] == pieceId[geo.get_point(x + 1, y + 1)]);
        painter.save();
        painter.translate((x + 0.025) * gridWidth, (y + 0.025) * gridHeight);
        painter.scale(0.95 * gridWidth, 0.95 * gridHeight);
        if (! hasLeft && ! hasRight && ! hasUp && ! hasDown)
            paintCallistoOnePiece(painter, 0, 0, 1, 1,
                                  base[c], light[c], dark[c]);
        else
        {
            auto d = 0.05 / 0.95;
            if (hasRight)
                painter.fillRect(QRectF(1, 0, d, 1), base[c]);
            if (hasDown)
                painter.fillRect(QRectF(0, 1, 1, d), base[c]);
            if (hasRightDown)
                painter.fillRect(QRectF(1, 1, d, d), base[c]);
            paintSquare(painter, 0, 0, 1, 1, base[c], light[c], dark[c]);
        }
        painter.restore();
    }
}

void paintPiecesClassic(
        QPainter& painter, qreal width, qreal height, const Geometry& geo,
        const Grid<PointState>& pointState, const ColorMap<QColor>& base,
        const ColorMap<QColor>& light, const ColorMap<QColor>& dark)
{
    auto gridWidth = width / geo.get_width();
    auto gridHeight = height / geo.get_height();
    for (auto p : geo)
    {
        if (pointState[p].is_empty())
            continue;
        auto c = pointState[p].to_color();
        paintSquare(painter, geo.get_x(p) * gridWidth,
                    geo.get_y(p) * gridHeight, gridWidth, gridHeight, base[c],
                    light[c], dark[c]);
    }
}

void paintPiecesGembloQ(
        QPainter& painter, qreal width, qreal height, const Geometry& geo,
        const Grid<PointState>& pointState, const ColorMap<QColor>& base,
        const ColorMap<QColor>& light, const ColorMap<QColor>& dark)
{
    auto gridWidth = width / geo.get_width();
    auto gridHeight = height / geo.get_height();
    for (auto p : geo)
    {
        if (pointState[p].is_empty())
            continue;
        auto c = pointState[p].to_color();
        painter.save();
        painter.translate(QPointF(geo.get_x(p) * gridWidth,
                                  geo.get_y(p) * gridHeight));
        QColor border;
        switch (geo.get_point_type(p))
        {
        case 0:
            border = light[c];
            break;
        case 1:
            border = dark[c];
            painter.rotate(180);
            painter.translate(-gridWidth, -gridHeight);
            break;
        case 2:
            border = dark[c];
            painter.rotate(270);
            painter.translate(-gridHeight, 0);
            break;
        case 3:
            border = light[c];
            painter.rotate(90);
            painter.translate(0, -gridWidth);
            break;
        }
        // Antialiasing cause unwanted seams between quarter squares
        painter.setRenderHint(QPainter::Antialiasing, false);
        paintQuarterSquareBase(painter, 0, 0, 2 * gridWidth, gridHeight,
                               base[c]);
        painter.setRenderHint(QPainter::Antialiasing);
        paintQuarterSquareFrame(painter, 0, 0, 2 * gridWidth, gridHeight,
                           border);
        painter.restore();
    }
}

void paintJunction(QPainter& painter, const Geometry& geo,
                   const Grid<PointState>& pointState,
                   const Grid<unsigned>& pieceId, Point p, qreal gridWidth,
                   qreal gridHeight, const ColorMap<QColor>& base)
{
    auto x = geo.get_x(p);
    auto y = geo.get_y(p);
    ArrayList<unsigned, 4> pieces;
    if (x > 0)
    {
        auto piece = pieceId[geo.get_point(x - 1, y)];
        if (piece != 0)
            pieces.include(piece);
    }
    if (x < geo.get_width() - 1)
    {
        auto piece = pieceId[geo.get_point(x + 1, y)];
        if (piece != 0)
            pieces.include(piece);
    }
    if (y > 0)
    {
        auto piece = pieceId[geo.get_point(x, y - 1)];
        if (piece != 0)
            pieces.include(piece);
    }
    if (y < geo.get_height() - 1)
    {
        auto piece = pieceId[geo.get_point(x, y + 1)];
        if (piece != 0)
            pieces.include(piece);
    }
    for (auto piece : pieces)
    {
        Color c;
        bool hasLeft = false;
        if (x > 0)
        {
            auto p = geo.get_point(x - 1, y);
            if (pieceId[p] == piece)
            {
                hasLeft = true;
                c = pointState[p].to_color();
            }
        }
        bool hasRight = false;
        if (x < geo.get_width() - 1)
        {
            auto p = geo.get_point(x + 1, y);
            if (pieceId[p] == piece)
            {
                hasRight = true;
                c = pointState[p].to_color();
            }
        }
        bool hasUp = false;
        if (y > 0)
        {
            auto p = geo.get_point(x, y - 1);
            if (pieceId[p] == piece)
            {
                hasUp = true;
                c = pointState[p].to_color();
            }
        }
        bool hasDown = false;
        if (y < geo.get_height() - 1)
        {
            auto p = geo.get_point(x, y + 1);
            if (pieceId[p] == piece)
            {
                hasDown = true;
                c = pointState[p].to_color();
            }
        }
        auto w = 0.5 * gridWidth;
        auto h = 0.5 * gridHeight;
        painter.save();
        painter.translate(x * gridWidth, y * gridHeight);
        if (hasLeft && hasRight && hasUp && hasDown)
            paintJunctionAll(painter, 0, 0, w, h, base[c]);
        else if (hasLeft && hasRight && ! hasUp && ! hasDown)
            paintJunctionStraight(painter, 0, 0, w, h, base[c]);
        else if (! hasLeft && ! hasRight && hasUp && hasDown)
        {
            painter.save();
            painter.rotate(90);
            painter.translate(0, -w);
            paintJunctionStraight(painter, 0, 0, w, h, base[c]);
            painter.restore();
        }
        else if (! hasLeft && hasRight && ! hasUp && hasDown)
            paintJunctionRight(painter, 0, 0, w, h, base[c]);
        else if (hasLeft && ! hasRight && ! hasUp && hasDown)
        {
            painter.save();
            painter.rotate(90);
            painter.translate(0, -w);
            paintJunctionRight(painter, 0, 0, w, h, base[c]);
            painter.restore();
        }
        else if (hasLeft && ! hasRight && hasUp && ! hasDown)
        {
            painter.save();
            painter.rotate(180);
            painter.translate(-w, -h);
            paintJunctionRight(painter, 0, 0, w, h, base[c]);
            painter.restore();
        }
        else if (! hasLeft && hasRight && hasUp && ! hasDown)
        {
            painter.save();
            painter.rotate(270);
            painter.translate(-h, 0);
            paintJunctionRight(painter, 0, 0, w, h, base[c]);
            painter.restore();
        }
        else if (hasLeft && hasRight && ! hasUp && hasDown)
            paintJunctionT(painter, 0, 0, w, h, base[c]);
        else if (hasLeft && ! hasRight && hasUp && hasDown)
        {
            painter.save();
            painter.rotate(90);
            painter.translate(0, -w);
            paintJunctionT(painter, 0, 0, w, h, base[c]);
            painter.restore();
        }
        else if (hasLeft && hasRight && hasUp && ! hasDown)
        {
            painter.save();
            painter.rotate(180);
            painter.translate(-w, -h);
            paintJunctionT(painter, 0, 0, w, h, base[c]);
            painter.restore();
        }
        else if (! hasLeft && hasRight && hasUp && hasDown)
        {
            painter.save();
            painter.rotate(270);
            painter.translate(-h, 0);
            paintJunctionT(painter, 0, 0, w, h, base[c]);
            painter.restore();
        }
        painter.restore();
    }
}

void paintPiecesNexos(
        QPainter& painter, qreal width, qreal height, const Geometry& geo,
        const Grid<PointState>& pointState, const Grid<unsigned>& pieceId,
        const ColorMap<QColor>& base, const ColorMap<QColor>& light,
        const ColorMap<QColor>& dark)
{
    auto gridWidth = width / (geo.get_width() - 0.5);
    auto gridHeight = height / (geo.get_height() - 0.5);
    for (auto p : geo)
    {
        switch (geo.get_point_type(p))
        {
        case 0:
            paintJunction(painter, geo, pointState, pieceId, p, gridWidth,
                          gridHeight, base);
            break;
        case 1:
        {
            if (pointState[p].is_empty())
                continue;
            auto c = pointState[p].to_color();
            paintSquare(painter, geo.get_x(p) * gridWidth - 0.5 * gridWidth,
                        geo.get_y(p) * gridHeight, 1.5 * gridWidth,
                        0.5 * gridHeight, base[c], light[c], dark[c]);
            break;
        }
        case 2:
        {
            if (pointState[p].is_empty())
                continue;
            auto c = pointState[p].to_color();
            paintSquare(painter, geo.get_x(p) * gridWidth,
                        geo.get_y(p) * gridHeight - 0.5 * gridHeight,
                        0.5 * gridWidth, 1.5 * gridHeight, base[c], light[c],
                        dark[c]);
            break;
        }
        }
    }
}

void paintPiecesTrigon(
        QPainter& painter, qreal width, qreal height, const Geometry& geo,
        const Grid<PointState>& pointState, const ColorMap<QColor>& base,
        const ColorMap<QColor>& light, const ColorMap<QColor>& dark)
{
    auto gridWidth = width / (geo.get_width() + 1);
    auto gridHeight = height / geo.get_height();
    for (auto p : geo)
    {
        if (pointState[p].is_empty())
            continue;
        auto c = pointState[p].to_color();
        if (geo.get_point_type(p) == 0)
            paintTriangleUp(painter, geo.get_x(p) * gridWidth - 0.5,
                            geo.get_y(p) * gridHeight, 2 * gridWidth,
                            gridHeight, base[c], light[c], dark[c]);
        else
            paintTriangleDown(painter, geo.get_x(p) * gridWidth - 0.5,
                              geo.get_y(p) * gridHeight, 2 * gridWidth,
                              gridHeight, base[c], light[c], dark[c]);
    }
}

} // namespace

//-----------------------------------------------------------------------------

void paint(QPainter& painter, qreal width, qreal height, Variant variant,
           const Geometry& geo, const Grid<PointState>& pointState,
           const Grid<unsigned>& pieceId)
{
    const QColor boardBase(174, 167, 172);
    const QColor boardLight(199, 191, 197);
    const QColor boardDark(134, 128, 132);
    const QColor centerBase(145, 139, 143);
    const QColor centerLight(160, 154, 159);
    const QColor centerDark(124, 119, 123);
    painter.setRenderHint(QPainter::Antialiasing);
    paintBoard(painter, width, height, variant, boardBase, boardLight,
               boardDark, centerBase, centerLight, centerDark);
    array<QColor, 3> blue{ {
        QColor(0, 115, 207), QColor(20, 153, 255), QColor(0, 72, 129)} };
    array<QColor, 3> green{ {
        QColor(0, 192, 0), QColor(0, 250, 0), QColor(0, 120, 0)} };
    array<QColor, 3> orange{ {
        QColor(240, 146, 23), QColor(255, 187, 103), QColor(157, 94, 11)} };
    array<QColor, 3> purple{ {
        QColor(161, 44, 207), QColor(190, 112, 220), QColor(109, 39, 135)} };
    array<QColor, 3> red{ {
        QColor(230, 62, 44), QColor(255, 101, 90), QColor(144, 38, 27)} };
    array<QColor, 3> yellow{ {
        QColor(245, 195, 32), QColor(255, 219, 88), QColor(170, 133, 22)} };
    ColorMap<QColor> piecesBase;
    ColorMap<QColor> piecesLight;
    ColorMap<QColor> piecesDark;
    if (variant == Variant::duo)
    {
        piecesBase[Color(0)] = purple[0];
        piecesLight[Color(0)] = purple[1];
        piecesDark[Color(0)] = purple[2];
    }
    else if (variant == Variant::junior)
    {
        piecesBase[Color(0)] = green[0];
        piecesLight[Color(0)] = green[1];
        piecesDark[Color(0)] = green[2];
    }
    else
    {
        piecesBase[Color(0)] = blue[0];
        piecesLight[Color(0)] = blue[1];
        piecesDark[Color(0)] = blue[2];
    }
    if (variant == Variant::duo || variant == Variant::junior)
    {
        piecesBase[Color(1)] = orange[0];
        piecesLight[Color(1)] = orange[1];
        piecesDark[Color(1)] = orange[2];
    }
    else if (get_nu_colors(variant) == 2)
    {
        piecesBase[Color(1)] = green[0];
        piecesLight[Color(1)] = green[1];
        piecesDark[Color(1)] = green[2];
    }
    else
    {
        piecesBase[Color(1)] = yellow[0];
        piecesLight[Color(1)] = yellow[1];
        piecesDark[Color(1)] = yellow[2];
    }
    piecesBase[Color(2)] = red[0];
    piecesLight[Color(2)] = red[1];
    piecesDark[Color(2)] = red[2];
    piecesBase[Color(3)] = green[0];
    piecesLight[Color(3)] = green[1];
    piecesDark[Color(3)] = green[2];
    switch (get_geometry_type(variant))
    {
    case GeometryType::classic:
        paintPiecesClassic(painter, width, height, geo, pointState, piecesBase,
                           piecesLight, piecesDark);
        break;
    case GeometryType::trigon:
        paintPiecesTrigon(painter, width, height, geo, pointState, piecesBase,
                          piecesLight, piecesDark);
        break;
    case GeometryType::nexos:
        paintPiecesNexos(painter, width, height, geo, pointState, pieceId,
                         piecesBase, piecesLight, piecesDark);
        break;
    case GeometryType::callisto:
        paintPiecesCallisto(painter, width, height, geo, pointState, pieceId,
                            piecesBase, piecesLight, piecesDark);
        break;
    case GeometryType::gembloq:
        paintPiecesGembloQ(painter, width, height, geo, pointState, piecesBase,
                           piecesLight, piecesDark);
        break;
    }
}

void paintBoard(QPainter& painter, qreal width, qreal height, Variant variant,
                const QColor& base, const QColor& light, const QColor& dark,
                const QColor& centerBase, const QColor& centerLight,
                const QColor& centerDark)
{
    auto& geo = get_geometry(variant);
    switch (get_geometry_type(variant))
    {
    case GeometryType::classic:
        paintBoardClassic(painter, width, height, geo, base, light, dark);
        break;
    case GeometryType::trigon:
        paintBoardTrigon(painter, width, height, geo, base, light, dark);
        break;
    case GeometryType::nexos:
        paintBoardNexos(painter, width, height, geo, base, light, dark);
        break;
    case GeometryType::callisto:
        paintBoardCallisto(painter, width, height, geo, get_nu_colors(variant),
                           base, light, dark, centerBase, centerLight,
                           centerDark);
        break;
    case GeometryType::gembloq:
        paintBoardGembloQ(painter, width, height, geo, base, light, dark);
        break;
    }
}

void paintCallistoOnePiece(QPainter& painter, qreal x, qreal y, qreal width,
                           qreal height, const QColor& base,
                           const QColor& light, const QColor& dark)
{
    auto dx = 0.175 * width;
    auto dy = 0.175 * height;
    painter.fillRect(QRectF(x, y, width, dy), base);
    painter.fillRect(QRectF(x, y + height - dy, width, dy), base);
    painter.fillRect(QRectF(x, y, dx, height), base);
    painter.fillRect(QRectF(x + width - dx, y, dx, height), base);
    paintSquareFrame(painter, x, y, width, height, light, dark);
}

void paintJunctionAll(QPainter& painter, qreal x, qreal y, qreal width,
                      qreal height, const QColor& base)
{
    auto dx = 0.22 * width;
    auto dy = 0.22 * height;
    painter.fillRect(QRectF(x + dx, y, width - 2 * dx, height), base);
    painter.fillRect(QRectF(x, y + dy, width, height - 2 * dy), base);
}

void paintJunctionRight(QPainter& painter, qreal x, qreal y, qreal width,
                        qreal height, const QColor& base)
{
    auto dx = 0.3 * width;
    auto dy = 0.3 * height;
    const QPointF polygon[3] =
    {
        {x + dx, y + height},
        {x + width, y + height},
        {x + width, y + dy}
    };
    painter.setPen(Qt::NoPen);
    painter.setBrush(base);
    painter.drawConvexPolygon(polygon, 3);
}

void paintJunctionStraight(QPainter& painter, qreal x, qreal y, qreal width,
                           qreal height, const QColor& base)
{
    auto dy = 0.22 * height;
    painter.fillRect(QRectF(x, y + dy, width, height - 2 * dy), base);
}

void paintJunctionT(QPainter& painter, qreal x, qreal y, qreal width,
                    qreal height, const QColor& base)
{
    auto dx = 0.22 * width;
    auto dy = 0.22 * height;
    painter.fillRect(QRectF(x + dx, y + dy, width - 2 * dx, height - dy),
                     base);
    painter.fillRect(QRectF(x, y + dy, width, height - 2 * dy), base);
}

void paintQuarterSquare(QPainter& painter, qreal x, qreal y, qreal width,
                        qreal height, const QColor& base, const QColor& light)
{
    paintQuarterSquareBase(painter, x, y, width, height, base);
    paintQuarterSquareFrame(painter, x, y, width, height, light);
}

void paintSquare(QPainter& painter, qreal x, qreal y, qreal width,
                 qreal height, const QColor& base, const QColor& light,
                 const QColor& dark)
{
    painter.fillRect(QRectF(x, y, width, height), base);
    paintSquareFrame(painter, x, y, width, height, light, dark);
}

void paintTriangleDown(QPainter& painter, qreal x, qreal y, qreal width,
                       qreal height, const QColor& base, const QColor& light,
                       const QColor& dark)
{
    const QPointF polygon[3] =
    {
        {x, y},
        {x + width, y},
        {x + 0.5 * width, y + height}
    };
    painter.setPen(Qt::NoPen);
    painter.setBrush(base);
    painter.drawConvexPolygon(polygon, 3);
    paintTriangleDownFrame(painter, x, y, width, height, light, dark);
}

void paintTriangleUp(QPainter& painter, qreal x, qreal y, qreal width,
                     qreal height, const QColor& base, const QColor& light,
                     const QColor& dark)
{
    const QPointF polygon[3] =
    {
        {x, y + height},
        {x + width, y + height},
        {x + 0.5 * width, y}
    };
    painter.setPen(Qt::NoPen);
    painter.setBrush(base);
    painter.drawConvexPolygon(polygon, 3);
    paintTriangleUpFrame(painter, x, y, width, height, light, dark);
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_paint
