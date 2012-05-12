//-----------------------------------------------------------------------------
/** @file pentobi/Util.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Util.h"

//-----------------------------------------------------------------------------

namespace Util
{

void removeThumbnail(const QString& file)
{
    // Note: in the future, it might be possible to trigger a thumbnail
    // update via D-Bus instead of removing it, but this is not yet
    // implemented in Gnome
    QFileInfo info(file);
    QString canonicalFile = info.canonicalFilePath();
    if (canonicalFile.isEmpty())
        canonicalFile = info.absoluteFilePath();
    QByteArray url = QUrl::fromLocalFile(canonicalFile).toEncoded();
    QByteArray md5 =
        QCryptographicHash::hash(url, QCryptographicHash::Md5).toHex();
    QString home = QDir::home().path();
    QFile::remove(home + "/.thumbnails/normal/" + md5 + ".png");
    QFile::remove(home + "/.thumbnails/large/" + md5 + ".png");
    QFile::remove(home + "/.thumbnails/fail/" + md5 + ".png");
}

} // namespace Util

//-----------------------------------------------------------------------------
