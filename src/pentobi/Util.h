//-----------------------------------------------------------------------------
/** @file pentobi/Util.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_GUI_UTIL_H
#define PENTOBI_GUI_UTIL_H

#include <QtGui>

//-----------------------------------------------------------------------------

namespace Util
{

/** Remove a thumbnail for a given file.
    Currently, the QT open file dialog shows thumbnails even if they belong
    to old versions of a file (see QTBUG-24724). This function can be used
    to remove an out-of-date freedesktop.org thumbnail if we know a file has
    changed (e.g. after saving). */
void removeThumbnail(const QString& file);

}

//-----------------------------------------------------------------------------

#endif // PENTOBI_GUI_UTIL_H
