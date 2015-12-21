//-----------------------------------------------------------------------------
/** @file libpentobi_gui/Util.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_UTIL_H
#define LIBPENTOBI_GUI_UTIL_H

#include <QColor>
#include <QPainter>
#include "libpentobi_base/Color.h"
#include "libpentobi_base/Variant.h"
#include "libpentobi_base/PointState.h"

using namespace std;
using libpentobi_base::Color;
using libpentobi_base::Variant;
using libpentobi_base::PointState;

//-----------------------------------------------------------------------------

namespace Util
{

QColor getPaintColor(Variant variant, Color c);

QColor getLabelColor(Variant variant, PointState s);

QColor getMarkColor(Variant variant, PointState s);

/** Paint piece segment in Nexos. */
void paintColorSegment(QPainter& painter, Variant variant, Color c,
                       bool isHorizontal, qreal x, qreal y, qreal size,
                       qreal alpha = 1, qreal saturation = 1,
                       bool flat = false);

void paintColorSquare(QPainter& painter, Variant variant, Color c,
                      qreal x, qreal y, qreal size, qreal alpha = 1,
                      qreal saturation = 1, bool flat = false);

void paintColorTriangle(QPainter& painter, Variant variant,
                        Color c, bool isUpward, qreal x, qreal y, qreal width,
                        qreal height, qreal alpha = 1, qreal saturation = 1,
                        bool flat = false);

/** Paint empty junction in Nexos. */
void paintEmptyJunction(QPainter& painter, qreal x, qreal y, qreal size);

/** Paint empty segment in Nexos. */
void paintEmptySegment(QPainter& painter, bool isHorizontal, qreal x, qreal y,
                       qreal size);

/** Paint empty segment with starting point in Nexos. */
void paintEmptySegmentStartingPoint(QPainter& painter, Variant variant,
                                    Color c, bool isHorizontal, qreal x,
                                    qreal y, qreal size);

void paintEmptySquare(QPainter& painter, qreal x, qreal y, qreal size);

void paintEmptySquareStartingPoint(QPainter& painter, Variant variant,
                                   Color c, qreal x, qreal y, qreal size);

void paintEmptyTriangle(QPainter& painter, bool isUpward, qreal x, qreal y,
                        qreal width, qreal height);

void paintEmptyTriangleStartingPoint(QPainter& painter, bool isUpward, qreal x,
                                     qreal y, qreal width, qreal height);

void paintJunction(QPainter& painter, Variant variant, Color c, qreal x,
                   qreal y, qreal width, qreal height, bool hasLeft,
                   bool hasRight, bool hasUp, bool hasDown, qreal alpha = 1,
                   qreal saturation = 1);

/** Convert a property value of a SGF tree unto a QString.
    @param value
    @param charset The value of the CA property of the root node in the tree
    or an empty string if the tree has no such property.
    This function currently only recognizes UTF8 and ISO-8859-1 (the latter
    is the default for SGF if no CA property exists). Other charsets are
    ignored and the string is converted using the default system charset. */
string convertSgfValueFromQString(const QString& value, const string& charset);

/** Convert a property value of a SGF tree unto a QString.
    @param value
    @param charset The value of the CA property of the root node in the tree
    or an empty string if the tree has no such property.
    This function currently only recognizes UTF8 and ISO-8859-1 (the latter
    is the default for SGF if no CA property exists). Other charsets are
    ignored and the string is converted using the default system charset. */
QString convertSgfValueToQString(const string& value, const string& charset);

/** Get a translated string identifying a player, like "Blue" or "Blue/Red".
    @param variant The game variant
    @param c The player color or one of the player colors in game variants
    with multiple colors per player. */
QString getPlayerString(Variant variant, Color c);

}

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_UTIL_H
