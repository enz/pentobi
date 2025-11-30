//-----------------------------------------------------------------------------
/** @file pentobi/Paint.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "Paint.h"

#include <QPainter>
#include "libpentobi_base/CallistoGeometry.h"

using namespace std;
using libboardgame_base::ArrayList;
using libpentobi_base::CallistoGeometry;
using libpentobi_base::Color;
using libpentobi_base::GeometryType;
using libpentobi_base::Point;

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
    qreal distX;
    qreal distY;
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

} // namespace

//-----------------------------------------------------------------------------

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
