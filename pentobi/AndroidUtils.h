//-----------------------------------------------------------------------------
/** @file pentobi/AndroidUtils.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_ANDROID_UTILS_H
#define PENTOBI_ANDROID_UTILS_H

#include <QObject>
#include <QUrl>

//-----------------------------------------------------------------------------

class AndroidUtils
    : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    Q_INVOKABLE static bool checkExists(const QString& file);

    Q_INVOKABLE static QUrl extractHelp(const QString& language);

    Q_INVOKABLE static QUrl getDefaultFolder();

    Q_INVOKABLE static QString getDisplayName(const QString& uri);

    Q_INVOKABLE void openImageSaveDialog(const QString& suggestedName);

    Q_INVOKABLE void openOpenDialog();

    Q_INVOKABLE void openTextSaveDialog();

    Q_INVOKABLE void openSaveDialog(const QString& suggestedName);

    Q_INVOKABLE static void releasePersistableUriPermission(
            const QString& uri);

    Q_INVOKABLE static bool save(const QString& uri, const QByteArray& array);

    Q_INVOKABLE static bool saveImage(const QString& uri,
                                      const QVariant& image);

#ifdef Q_OS_ANDROID
    /** Return the logical density of the display.
        Returns android.util.DisplayMetrics.density. This should be the same as
        Screen.devicePixelRatio, but can be used before QGuiApplication is
        constructed.
        @return The density or 0 on error. */
    static float getDensity();

    static QString getError() { return m_error; }

    static QString getInitialFile();

    static bool open(const QString& uri, QByteArray& sgf);
#endif

signals:
    void imageSaveDialogAccepted(const QString& uri);

    void openDialogAccepted(const QString& uri, const QString& displayName);

    void saveDialogAccepted(const QString& uri, const QString& displayName);

    void textSaveDialogAccepted(const QString& uri);

private:
    static QString m_error;


#ifdef Q_OS_ANDROID
    static bool checkException();
#endif
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_ANDROID_UTILS_H
