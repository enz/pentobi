//-----------------------------------------------------------------------------
/** @file pentobi/Util.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_UTIL_H
#define PENTOBI_UTIL_H

#include <QtGui>
#include "libboardgame_base/Rating.h"
#include "libpentobi_base/Color.h"
#include "libpentobi_base/Variant.h"

using libboardgame_base::Rating;
using libpentobi_base::Color;
using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

namespace Util
{

/** Remove a thumbnail for a given file.
    Currently, the QT open file dialog shows thumbnails even if they belong
    to old versions of a file (see QTBUG-24724). This function can be used
    to remove an out-of-date freedesktop.org thumbnail if we know a file has
    changed (e.g. after saving). */
void removeThumbnail(const QString& file);

void getRating(Variant variant, Rating& rating, unsigned int& nuGames,
               Rating& bestRating);

void initRating(Variant variant, Rating rating);

void updateRating(Variant variant, float score, Rating opponentRating,
                  unsigned int nuOpponents);

void getNextRatedGameSettings(Variant variant, int maxLevel, int& level,
                              Color& userColor);

}

//-----------------------------------------------------------------------------

#endif // PENTOBI_UTIL_H
