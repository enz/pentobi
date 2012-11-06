//-----------------------------------------------------------------------------
/** @file pentobi/Util.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Util.h"

#include <QCryptographicHash>
#include <QDesktopServices>
#include <QDialog>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QUrl>
#include "libpentobi_mcts/Player.h"

using libpentobi_mcts::Player;

//-----------------------------------------------------------------------------

namespace Util
{

QString getDataDir()
{
    return QDesktopServices::storageLocation(QDesktopServices::DataLocation);
}

void initDataDir()
{
    QString dataLocation = getDataDir();
    QDir dir(dataLocation);
    if (! dir.exists())
        // Note: dataLocation is an absolute path but there is no static
        // function QDir::mkpath()
        dir.mkpath(dataLocation);

    // Check for locations used by previous versions of Pentobi
#ifdef Q_WS_WIN
    QString oldLocation = QDir::home().path() + "/AppData/Roaming/Pentobi";
#else
    QString oldLocation;
    const char* xdgDataHome = getenv("XDG_DATA_HOME");
    if (xdgDataHome != 0)
        oldLocation = QString(xdgDataHome) + "/pentobi";
    else
        oldLocation = QDir::home().path() + "/.local/share/pentobi";
#endif
    if (QDir(oldLocation).exists())
    {
        if (dir.rmdir(dataLocation))
            QFile::rename(oldLocation, dataLocation);
    }
}

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
}

void setNoTitle(QDialog& dialog)
{
    // On many platforms, message boxes should have no title but using
    // an emtpy string causes Qt to use the lower-case application name (tested
    // on Linux with Qt 4.8). As a workaround, we set the title to a space
    // character.
    dialog.setWindowTitle(" ");
}

} // namespace Util

//-----------------------------------------------------------------------------
