//-----------------------------------------------------------------------------
/** @file pentobi/Util.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_UTIL_H
#define PENTOBI_UTIL_H

#include "RatingHistory.h"
#include "libboardgame_base/Rating.h"
#include "libpentobi_base/Color.h"
#include "libpentobi_base/Variant.h"

class QDialog;
class QString;

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

/** Return the platform-dependent directory for storing data for the current
    application. */
QString getDataDir();

/** Create the platform-dependent directory for storing data for the current
    application if it does not exist yet. */
void initDataDir();

/** Set an empty window title for message boxes and similar small dialogs. */
void setNoTitle(QDialog& dialog);

}

//-----------------------------------------------------------------------------

#endif // PENTOBI_UTIL_H
