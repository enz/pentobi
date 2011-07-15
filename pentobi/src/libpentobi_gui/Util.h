//-----------------------------------------------------------------------------
/** @file libpentobi_gui/Util.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_UTIL_H
#define LIBPENTOBI_GUI_UTIL_H

#include <QtGui>
#include "libpentobi_base/Color.h"
#include "libpentobi_base/GameVariant.h"
#include "libpentobi_base/PointState.h"

using libpentobi_base::Color;
using libpentobi_base::GameVariant;
using libpentobi_base::PointState;

//-----------------------------------------------------------------------------

namespace Util
{

QColor getPaintColor(GameVariant gameVariant, Color c);

QColor getPaintColorEmpty();

QColor getLabelColor(GameVariant gameVariant, PointState s);

void paintColorSquare(QPainter& painter, GameVariant gameVariant, Color c,
                      int x, int y, int size);

void paintEmptySquare(QPainter& painter, int x, int y, int size);

void paintEmptySquareStartingPoint(QPainter& painter, GameVariant gameVariant,
                                   Color c, int x, int y, int size);

}

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_UTIL_H
