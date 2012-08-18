//-----------------------------------------------------------------------------
/** @file libpentobi_thumbnail/CreateThumbnail.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_THUMBNAIL_CREATE_THUMBNAIL_H
#define LIBPENTOBI_THUMBNAIL_CREATE_THUMBNAIL_H

#include <QtGui>

//-----------------------------------------------------------------------------

bool createThumbnail(const QString& path, int width, int height,
                     QImage& image);

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_THUMBNAIL_CREATE_THUMBNAIL_H
