//-----------------------------------------------------------------------------
/** @file pentobi/AndroidUtils.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "AndroidUtils.h"

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QtAndroid>
#include <QtAndroidExtras/QAndroidJniObject>
#endif

//-----------------------------------------------------------------------------

bool AndroidUtils::checkPermission(const QString& permission)
{
#ifdef Q_OS_ANDROID
    return QtAndroid::checkPermission(permission) ==
           QtAndroid::PermissionResult::Granted;
#else
    Q_UNUSED(permission);
    return true;
#endif
}

void AndroidUtils::scanFile(const QString& pathname)
{
#ifdef Q_OS_ANDROID
    // Corresponding Java code:
    //   sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE,
    //                       Uri.fromFile(File(pathname).getCanonicalFile())));
    auto ACTION_MEDIA_SCANNER_SCAN_FILE =
            QAndroidJniObject::getStaticObjectField<jstring>(
                "android/content/Intent", "ACTION_MEDIA_SCANNER_SCAN_FILE");
    if (! ACTION_MEDIA_SCANNER_SCAN_FILE.isValid())
        return;
    auto pathnameString = QAndroidJniObject::fromString(pathname);
    QAndroidJniObject file("java/io/File", "(Ljava/lang/String;)V",
                           pathnameString.object<jstring>());
    if (! file.isValid())
        return;
    auto absoluteFile = file.callObjectMethod(
                "getCanonicalFile", "()Ljava/io/File;", file.object());
    if (! absoluteFile.isValid())
        return;
    auto uri = QAndroidJniObject::callStaticObjectMethod(
                "android/net/Uri", "fromFile",
                "(Ljava/io/File;)Landroid/net/Uri;", absoluteFile.object());
    if (! uri.isValid())
        return;
    QAndroidJniObject intent("android/content/Intent",
                             "(Ljava/lang/String;Landroid/net/Uri;)V",
                             ACTION_MEDIA_SCANNER_SCAN_FILE.object<jstring>(),
                             uri.object());
    if (! intent.isValid())
        return;
    auto activity = QtAndroid::androidActivity();
    activity.callMethod<void>("sendBroadcast", "(Landroid/content/Intent;)V",
                              intent.object());
#else
    Q_UNUSED(pathname);
#endif
}

//-----------------------------------------------------------------------------
