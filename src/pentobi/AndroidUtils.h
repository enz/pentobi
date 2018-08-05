//-----------------------------------------------------------------------------
/** @file pentobi/AndroidUtils.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_ANDROID_UTILS_H
#define PENTOBI_ANDROID_UTILS_H

// Needed in the header because moc_*.cpp does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QObject>

#include <QUrl>

//-----------------------------------------------------------------------------

class AndroidUtils
    : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    /** Calls QtAndroid::checkPermission().
        On platforms other than Android, always returns true. */
    Q_INVOKABLE static bool checkPermission(const QString& permission);

    Q_INVOKABLE QUrl extractHelp(const QString& language);

    /** Return a directory for storing files.
        Avoids a dependency on qt.labs.platform only for StandardPaths and
        handles Android better. On Android, it returns
        android.os.Environment.getExternalStorageDirectory(). On other
        platforms, it returns QStandardPaths::HomeLocation */
    Q_INVOKABLE static QUrl getDefaultFolder();

    /** Request the Android media scanner to scan a file.
        Ensures that the file will be visible via MTP. On platforms other
        than Android, this function does nothing. */
    Q_INVOKABLE static void scanFile(const QString& pathname);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_ANDROID_UTILS_H
