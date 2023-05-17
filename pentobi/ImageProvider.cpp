//-----------------------------------------------------------------------------
/** @file pentobi/ImageProvider.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "ImageProvider.h"

#include <QPainter>
#include "libpentobi_paint/Paint.h"

using namespace std;
using namespace Qt::StringLiterals;
using namespace libpentobi_paint;

//-----------------------------------------------------------------------------

ImageProvider::ImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{ }

QPixmap ImageProvider::requestPixmap(const QString& id, QSize* size,
                                     const QSize& requestedSize)
{
    // Piece element images are always created with a user-defined sourceSize,
    // the image might be requested temporarily with a useless sourceSize, for
    // example 0 or negative if scaleUnplayed of a piece is 0, or width 1 but
    // height greater 1 for a square because the width and height properties
    // are updated in two steps. In these cases, we return a 1x1 pixmap (0x0
    // would cause a QQuickImageProvider warning).
    int width = max(requestedSize.width(), 1);
    int height = max(requestedSize.height(), 1);
    *size = QSize(width, height);
    QPixmap pixmap(width, height);
    if (requestedSize.width() <= 1 || requestedSize.height() <= 1)
        return pixmap;
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    auto elem = id.split('/');
    if (elem.empty())
        return pixmap;
    auto name = elem[0];
    if (name == "board"_L1 && elem.size() == 8)
    {
        auto gameVariant = elem[1].toLocal8Bit();
        QColor base(elem[2]);
        QColor dark(elem[3]);
        QColor light(elem[4]);
        QColor centerBase(elem[5]);
        QColor centerDark(elem[6]);
        QColor centerLight(elem[7]);
        Variant variant;
        if (parse_variant_id(gameVariant.constData(), variant))
            paintBoard(painter, width, height, variant, base, light, dark,
                       centerBase, centerLight, centerDark);
    }
    else if (elem.size() == 2)
    {
        QColor base(elem[1]);
        if (name == "junction-all"_L1)
            paintJunctionAll(painter, 0, 0, width, height, base);
        else if (name == "junction-right"_L1)
            paintJunctionRight(painter, 0, 0, width, height, base);
        else if (name == "junction-straight"_L1)
            paintJunctionStraight(painter, 0, 0, width, height, base);
        else if (name == "junction-t"_L1)
            paintJunctionT(painter, 0, 0, width, height, base);
    }
    else if (name == "quarter-square"_L1 && elem.size() == 3)
    {
        QColor base(elem[1]);
        QColor light(elem[2]);
        paintQuarterSquare(painter, 0, 0, width, height, base, light);
    }
    else if (elem.size() == 4)
    {
        QColor base(elem[1]);
        QColor dark(elem[2]);
        QColor light(elem[3]);
        if (name == "frame"_L1)
            paintCallistoOnePiece(painter, 0, 0, width, height, base, light,
                                  dark);
        else if (name == "square"_L1)
            paintSquare(painter, 0, 0, width, height, base, light, dark);
        else if (name == "triangle"_L1)
            paintTriangleUp(painter, 0, 0, width, height, base, light, dark);
        else if (name == "triangle-down"_L1)
            paintTriangleDown(painter, 0, 0, width, height, base, light, dark);
    }
    return pixmap;
}

//-----------------------------------------------------------------------------
