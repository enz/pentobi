//-----------------------------------------------------------------------------
/** @file pentobi/AndroidUtils.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_ANDROID_UTILS_H
#define PENTOBI_ANDROID_UTILS_H

#include <QColor>
#include <QObject>
#include <QtQml/qqmlregistration.h>

//-----------------------------------------------------------------------------

class AndroidUtils
    : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    using QObject::QObject;

    Q_INVOKABLE static bool checkExists(const QString& file);

    Q_INVOKABLE static QString getDisplayName(const QString& uri);

    Q_INVOKABLE void openImageSaveDialog(const QString& suggestedName);

    Q_INVOKABLE void openOpenDialog(const QString& suggestedUri);

    /** Open a native file dialog for saving a game.
        Note that we cannot migrate this to QtQuick.Dialogs.FileDialog because
        we need to call ACTION_CREATE_DOCUMENT with type
        application/x-blokus-sgf and Qt automatically sets the type using
        QMimeDatabase, which knows only standard types. */
    Q_INVOKABLE void openSaveDialog(const QString& suggestedUri,
                                    const QString& suggestedName);

    Q_INVOKABLE static QStringList getPersistedUriPermissions();

    Q_INVOKABLE static void releasePersistableUriPermission(
            const QString& uri);

    Q_INVOKABLE static bool save(const QString& uri, const QByteArray& array);

    Q_INVOKABLE static bool saveImage(const QString& uri,
                                      const QVariant& image);

    /** Exit application and avoid crash due to QTBUG-85449.
        On Android, this exits the application with java.lang.System.exit(),
        which avoids crashes on some devices due to QTBUG-85449. Global
        destructors will not be called, but we cannot rely on that anyway
        because Android can also kill suspended apps without calling global
        destructors. Does nothing on other platforms. */
    Q_INVOKABLE void exit();

#ifdef Q_OS_ANDROID
    static QString getError() { return m_error; }

    static bool open(const QString& uri, QByteArray& sgf);
#endif

signals:
    void imageSaveDialogAccepted(const QString& uri);

    void openDialogAccepted(const QString& uri, const QString& displayName);

    void saveDialogAccepted(const QString& uri, const QString& displayName);

private:
    static QString m_error;


#ifdef Q_OS_ANDROID
    static bool checkException();
#endif
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_ANDROID_UTILS_H
