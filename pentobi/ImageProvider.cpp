//-----------------------------------------------------------------------------
/** @file pentobi/ImageProvider.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "ImageProvider.h"

#include <QPainter>
#include "libpentobi_paint/Paint.h"

using namespace std;
using namespace libpentobi_paint;

//-----------------------------------------------------------------------------

ImageProvider::ImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{ }

QPixmap ImageProvider::requestPixmap(const QString& id, QSize* size,
                                     const QSize& requestedSize)
{
    // Piece element images are always created with a user-defined sourceSize,
    // requestedSize can only become 0 temporarily when changing the game
    // variant (when scaleUnplayed of a piece becomes 0). In this case, we
    // return a 1x1 pixmap (0x0 would cause a QQuickImageProvider warning).
    int width = max(requestedSize.width(), 1);
    int height = max(requestedSize.height(), 1);
    *size = QSize(width, height);
    QPixmap pixmap(width, height);
    if (requestedSize.width() == 0 || requestedSize.height() == 0)
        return pixmap;
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    auto splitRef = id.splitRef(QStringLiteral("/"));
    if (splitRef.empty())
        return pixmap;
    auto name = splitRef[0];
    if (name == "board" && splitRef.size() == 8)
    {
        auto gameVariant = splitRef[1].toLocal8Bit();
        QColor base(splitRef[2]);
        QColor dark(splitRef[3]);
        QColor light(splitRef[4]);
        QColor centerBase(splitRef[5]);
        QColor centerDark(splitRef[6]);
        QColor centerLight(splitRef[7]);
        Variant variant;
        if (parse_variant_id(gameVariant.constData(), variant))
            paintBoard(painter, width, height, variant, base, light, dark,
                       centerBase, centerLight, centerDark);
    }
    else if (splitRef.size() == 2)
    {
        QColor base(splitRef[1]);
        if (name == "junction-all")
            paintJunctionAll(painter, 0, 0, width, height, base);
        else if (name == "junction-right")
            paintJunctionRight(painter, 0, 0, width, height, base);
        else if (name == "junction-straight")
            paintJunctionStraight(painter, 0, 0, width, height, base);
        else if (name == "junction-t")
            paintJunctionT(painter, 0, 0, width, height, base);
    }
    else if (splitRef.size() == 4)
    {
        QColor base(splitRef[1]);
        QColor dark(splitRef[2]);
        QColor light(splitRef[3]);
        if (name == "frame")
            paintCallistoOnePiece(painter, 0, 0, width, height, base, light,
                                  dark);
        else if (name == "quarter-square")
            paintQuarterSquare(painter, 0, 0, width, height, base, light);
        else if (name == "quarter-square-bottom")
            paintQuarterSquare(painter, 0, 0, width, height, base, dark);
        else if (name == "square")
            paintSquare(painter, 0, 0, width, height, base, light, dark);
        else if (name == "triangle")
            paintTriangleUp(painter, 0, 0, width, height, base, light, dark);
        else if (name == "triangle-down")
            paintTriangleDown(painter, 0, 0, width, height, base, light, dark);
    }
    return pixmap;
}

//-----------------------------------------------------------------------------
