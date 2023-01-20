//-----------------------------------------------------------------------------
/** @file pentobi_thumbnailer/CreateThumbnail.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_THUMBNAIL_CREATE_THUMBNAIL_H
#define LIBPENTOBI_THUMBNAIL_CREATE_THUMBNAIL_H

class QImage;
class QString;

//-----------------------------------------------------------------------------

bool createThumbnail(const QString& path, int width, int height,
                     QImage& image);

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_THUMBNAIL_CREATE_THUMBNAIL_H
