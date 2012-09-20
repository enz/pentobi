//-----------------------------------------------------------------------------
/** @file pentobi/Util.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Util.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QString>
#include <QUrl>
#include "libpentobi_mcts/Player.h"

using libpentobi_mcts::Player;

//-----------------------------------------------------------------------------

namespace Util
{

QString getDataDir()
{
    QString home = QDir::toNativeSeparators(QDir::home().path());
    QChar sep = QDir::separator();
    QString dir;
#ifdef Q_WS_WIN
    dir = home + sep + "AppData" + sep + "Roaming";
    if (! QDir(dir).exists("Pentobi") && ! QDir(dir).mkpath("Pentobi"))
        dir = home;
    else
        dir = dir + sep + "Pentobi";
#else
    const char* xdgDataHome = getenv("XDG_DATA_HOME");
    if (xdgDataHome != 0)
        dir = xdgDataHome;
    else
        dir = home + sep + ".local" + sep + "share";
    if (! QDir(dir).exists("pentobi") && ! QDir(dir).mkpath("pentobi"))
        dir = home;
    else
        dir = dir + sep + "pentobi";
#endif
    return dir;
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

} // namespace Util

//-----------------------------------------------------------------------------
