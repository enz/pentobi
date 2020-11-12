//-----------------------------------------------------------------------------
/** @file pentobi/AndroidUtils.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "AndroidUtils.h"

#include <QCoreApplication>
#include <QStandardPaths>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QAndroidJniExceptionCleaner>
#include <QBuffer>
#include <QDir>
#include <QDirIterator>
#include <QHash>
#include <QImage>
#include <QtAndroid>
#include <QVariant>
#else
#include <QFileInfo>
#endif

using namespace std;

//-----------------------------------------------------------------------------

namespace {

#ifdef Q_OS_ANDROID

struct AutoClose
{
    const QAndroidJniObject& m_obj;

    ~AutoClose()
    {
        m_obj.callMethod<void>("close", "()V");
    }
};

void takePersistableUriPermission(const QAndroidJniObject& intent,
                                  const QAndroidJniObject& uri);

QAndroidJniObject getContentResolver()
{
    return QtAndroid::androidActivity().callObjectMethod(
                "getContentResolver", "()Landroid/content/ContentResolver;");
}

QString getDisplayName(QAndroidJniObject uri)
{
    auto contentResolver = getContentResolver();
    if (! contentResolver.isValid())
        return {};
    QAndroidJniExceptionCleaner exceptionCleaner;
    QAndroidJniEnvironment env;
    auto stringClass = env->FindClass("java/lang/String");
    auto projection = env->NewObjectArray(1, stringClass, nullptr);
    auto column = QAndroidJniObject::getStaticObjectField<jstring>(
                "android/provider/OpenableColumns", "DISPLAY_NAME");
    if (! column.isValid())
        return {};
    env->SetObjectArrayElement(projection, 0, column.object());
    auto cursor = contentResolver.callObjectMethod(
                "query",
                "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;"
                "[Ljava/lang/String;Ljava/lang/String;)"
                "Landroid/database/Cursor;",
                uri.object(), projection, nullptr, nullptr, nullptr);
    if (env->ExceptionCheck())
        return {};
    AutoClose autoClose{cursor};
    if (! cursor.callMethod<jboolean>("moveToFirst", "()Z"))
        return {};
    auto index = cursor.callMethod<jint>(
                "getColumnIndex", "(Ljava/lang/String;)I", column.object());
    if (index < 0)
        return {};
    auto name = cursor.callObjectMethod(
                "getString", "(I)Ljava/lang/String;", index);
    if (! name.isValid())
        return {};
    return name.toString();
}

QAndroidJniObject getUriObj(const QString& uri)
{
    QAndroidJniExceptionCleaner exceptionCleaner;
    auto uriString = QAndroidJniObject::fromString(uri);
    return QAndroidJniObject::callStaticObjectMethod(
                "android/net/Uri", "parse",
                "(Ljava/lang/String;)Landroid/net/Uri;",
                uriString.object<jstring>());
}

void startDocumentActivity(
        const char* actionField, const QString& type,
        const QString& extraTitle, bool takePersistablePermission,
        const function<void(const QString& uri,const QString& displayNamei)>&
        callback)
{
    auto action = QAndroidJniObject::getStaticObjectField<jstring>(
                "android/content/Intent", actionField);
    if (! action.isValid())
        return;
    QAndroidJniObject intent("android/content/Intent", "(Ljava/lang/String;)V",
                             action.object<jstring>());
    if (! intent.isValid())
        return;
    auto category = QAndroidJniObject::getStaticObjectField<jstring>(
                "android/content/Intent", "CATEGORY_OPENABLE");
    if (! category.isValid())
        return;
    intent.callObjectMethod(
                "addCategory",
                "(Ljava/lang/String;)Landroid/content/Intent;",
                category.object<jstring>());
    auto typeObj = QAndroidJniObject::fromString(type);
    intent.callObjectMethod(
                "setType",
                "(Ljava/lang/String;)Landroid/content/Intent;",
                typeObj.object<jstring>());
    if (! extraTitle.isEmpty())
    {
        auto extraTitleObj = QAndroidJniObject::getStaticObjectField<jstring>(
                    "android/content/Intent", "EXTRA_TITLE");
        if (! extraTitleObj.isValid())
            return;
        auto extraTitleValue = QAndroidJniObject::fromString(extraTitle);
        intent.callObjectMethod(
                    "putExtra",
                    "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;",
                    extraTitleObj.object<jstring>(),
                    extraTitleValue.object<jstring>());
    }
    QtAndroid::startActivity(intent, 0,
                             [takePersistablePermission, callback](int, int
                             result, const QAndroidJniObject& data) {
        auto ok = QAndroidJniObject::getStaticField<jint>(
                    "android/app/Activity", "RESULT_OK");
        if (result != ok)
            return;
        if (! data.isValid())
            return;
        auto uri = data.callObjectMethod("getData", "()Landroid/net/Uri;");
        if (! uri.isValid())
            return;
        auto uriString =
                uri.callObjectMethod("toString", "()Ljava/lang/String;");
        if (! uriString.isValid())
            return;
        if (takePersistablePermission)
            takePersistableUriPermission(data, uri);
        auto displayName = getDisplayName(uri);
        callback(uriString.toString(), displayName);
    });
}

void takePersistableUriPermission(const QAndroidJniObject& intent,
                                  const QAndroidJniObject& uri)
{
    auto contentResolver = getContentResolver();
    if (! contentResolver.isValid())
        return;
    auto flags = intent.callMethod<jint>("getFlags", "()I");
    auto flagRead = QAndroidJniObject::getStaticField<jint>(
                "android/content/Intent", "FLAG_GRANT_READ_URI_PERMISSION");
    auto flagWrite = QAndroidJniObject::getStaticField<jint>(
                "android/content/Intent", "FLAG_GRANT_WRITE_URI_PERMISSION");
    contentResolver.callMethod<void>(
                "takePersistableUriPermission", "(Landroid/net/Uri;I)V",
                uri.object(), flags & (flagRead | flagWrite));
}

#endif

} // namespace

//-----------------------------------------------------------------------------

QString AndroidUtils::m_error;

#ifdef Q_OS_ANDROID
bool AndroidUtils::checkException()
{
    QAndroidJniEnvironment env;
    if (! env->ExceptionCheck())
        return false;
    auto e = env->ExceptionOccurred();
    env->ExceptionClear();
    auto method = env->GetMethodID(env->GetObjectClass(e),
                                   "getLocalizedMessage",
                                   "()Ljava/lang/String;");
    QAndroidJniObject message(env->CallObjectMethod(e, method));
    m_error = message.toString();
    return true;
}
#endif

bool AndroidUtils::checkExists(const QString& file)
{
#ifdef Q_OS_ANDROID
    auto contentResolver = getContentResolver();
    if (! contentResolver.isValid())
        return false;
    auto uriObj = getUriObj(file);
    if (! uriObj.isValid())
        return false;
    QAndroidJniExceptionCleaner exceptionCleaner;
    QAndroidJniEnvironment env;
    auto stringClass = env->FindClass("java/lang/String");
    auto projection = env->NewObjectArray(1, stringClass, nullptr);
    auto column = QAndroidJniObject::getStaticObjectField<jstring>(
                "android/provider/DocumentsContract$Document",
                "COLUMN_DOCUMENT_ID");
    if (! column.isValid())
        return false;
    env->SetObjectArrayElement(projection, 0, column.object());
    auto cursor = contentResolver.callObjectMethod(
                "query",
                "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;"
                "[Ljava/lang/String;Ljava/lang/String;)"
                "Landroid/database/Cursor;",
                uriObj.object(), projection, nullptr, nullptr, nullptr);
    if (env->ExceptionCheck())
        return false;
    if (! cursor.isValid())
        return false;
    AutoClose autoClose{cursor};
    auto count = cursor.callMethod<jint>("getCount", "()I");
    return count > 0;
#else
    return QFileInfo::exists(file);
#endif
}

QUrl AndroidUtils::extractHelp([[maybe_unused]] const QString& language)
{
#ifdef Q_OS_ANDROID
    if (language != QStringLiteral("C"))
        // Other languages use pictures from C
        extractHelp(QStringLiteral("C"));
    auto activity = QtAndroid::androidActivity();
    auto filesDir =
            activity.callObjectMethod("getFilesDir", "()Ljava/io/File;");
    if (! filesDir.isValid())
        return {};
    auto filesDirString = filesDir.callObjectMethod("toString",
                                                    "()Ljava/lang/String;");
    if (! filesDirString.isValid())
        return {};
    QDir dir(filesDirString.toString() + "/help/"
             + QCoreApplication::applicationVersion() + "/" + language
             + "/pentobi");
    auto dirPath = dir.path();
    if (QFileInfo::exists(dirPath + "/index.html"))
        return QUrl::fromLocalFile(dirPath + "/index.html");
    if (! QFileInfo::exists(filesDirString.toString() + "/help/"
                            + QCoreApplication::applicationVersion()
                            + "/C/pentobi/index.html"))
        // No need to keep files from older versions around
        QDir(filesDirString.toString() + "/help").removeRecursively();
    QDirIterator it(":qml/help/" + language + "/pentobi");
    while (it.hasNext())
    {
        it.next();
        if (! it.fileInfo().isFile())
            continue;
        QFile dest(dirPath + "/" + it.fileName());
        QFileInfo(dest).dir().mkpath(QStringLiteral("."));
        dest.remove();
        QFile::copy(it.filePath(), dest.fileName());
    }
    auto file = QFileInfo(dirPath + "/index.html").absoluteFilePath();
    return QUrl::fromLocalFile(file);
#else
    return {};
#endif
}

QUrl AndroidUtils::getDefaultFolder()
{
#ifdef Q_OS_ANDROID
    // We don't need to know a default folder on Android anymore since we
    // now use the Storage Access Framework.
    return {};
#else
    // We could also get the home directory Qt.labs.platform.StandardPaths but
    // we don't want a dependency on Qt.labs.platform for only one function.
    return QUrl::fromLocalFile(
                QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
#endif
}

QString AndroidUtils::getDisplayName([[maybe_unused]]const QString& uri)
{
#ifdef Q_OS_ANDROID
    if (! QUrl(uri).isRelative())
        return ::getDisplayName(getUriObj(uri));
#endif
    return {};
}

#ifdef Q_OS_ANDROID
QString AndroidUtils::getInitialFile()
{
    auto intent = QtAndroid::androidActivity().callObjectMethod(
                "getIntent", "()Landroid/content/Intent;");
    if (! intent.isValid())
        return {};
    auto uri = intent.callObjectMethod("getData", "()Landroid/net/Uri;");
    if (! uri.isValid())
        return {};
    auto uriString =
            uri.callObjectMethod("toString", "()Ljava/lang/String;");
    if (! uriString.isValid())
        return {};
    return uriString.toString();
}
#endif

#ifdef Q_OS_ANDROID
bool AndroidUtils::open(
        [[maybe_unused]]const QString& uri, [[maybe_unused]]QByteArray& sgf)
{
    m_error.clear();
    auto contentResolver = getContentResolver();
    if (! contentResolver.isValid())
        return false;
    auto uriObj = getUriObj(uri);
    if (! uriObj.isValid())
        return false;
    QAndroidJniExceptionCleaner exceptionCleaner;
    auto inputStream = contentResolver.callObjectMethod(
                "openInputStream",
                "(Landroid/net/Uri;)Ljava/io/InputStream;", uriObj.object());
    if (checkException())
        return false;
    if (! inputStream.isValid())
        return false;
    sgf.clear();
    QAndroidJniEnvironment env;
    const int bufSize = 2048;
    auto byteArray = env->NewByteArray(bufSize);
    jbyte buf[bufSize];
    jint n;
    while ((n = inputStream.callMethod<jint>("read", "([B)I", byteArray)) >= 0)
    {
        if (checkException())
        {
            exceptionCleaner.clean();
            inputStream.callMethod<void>("close", "()V");
            return false;
        }
        env->GetByteArrayRegion(byteArray, 0, n, buf);
        sgf.append(reinterpret_cast<const char*>(&buf[0]), n);
    }
    inputStream.callMethod<void>("close", "()V");
    return true;
}
#endif

void AndroidUtils::openImageSaveDialog(
        [[maybe_unused]] const QString& suggestedName)
{
#ifdef Q_OS_ANDROID
    startDocumentActivity("ACTION_CREATE_DOCUMENT", "image/png",
                          suggestedName, false,
                          [this](const QString& uri,
                          [[maybe_unused]]const QString& displayName) {
        emit imageSaveDialogAccepted(uri);
    });
#endif
}

void AndroidUtils::openOpenDialog()
{
#ifdef Q_OS_ANDROID
    startDocumentActivity("ACTION_OPEN_DOCUMENT", "*/*", "", true,
                          [this](const QString& uri,
                          const QString& displayName) {
        emit openDialogAccepted(uri, displayName);
    });
#endif
}

void AndroidUtils::openTextSaveDialog()
{
#ifdef Q_OS_ANDROID
    startDocumentActivity("ACTION_CREATE_DOCUMENT", "text/plain",
                          "", false,
                          [this](const QString& uri,
                          [[maybe_unused]]const QString& displayName) {
        emit textSaveDialogAccepted(uri);
    });
#endif
}

void AndroidUtils::openSaveDialog(
        [[maybe_unused]] const QString& suggestedName)
{
#ifdef Q_OS_ANDROID
    startDocumentActivity("ACTION_CREATE_DOCUMENT", "application/x-blokus-sgf",
                          suggestedName, true,
                          [this](const QString& uri,
                          const QString& displayName) {
        emit saveDialogAccepted(uri, displayName);
    });
#endif
}

void AndroidUtils::releasePersistableUriPermission(
        [[maybe_unused]]const QString& uri)
{
#ifdef Q_OS_ANDROID
    auto uriObj = getUriObj(uri);
    if (! uriObj.isValid())
        return;
    auto contentResolver = getContentResolver();
    if (! contentResolver.isValid())
        return;
    auto flagRead = QAndroidJniObject::getStaticField<jint>(
                "android/content/Intent", "FLAG_GRANT_READ_URI_PERMISSION");
    auto flagWrite = QAndroidJniObject::getStaticField<jint>(
                "android/content/Intent", "FLAG_GRANT_WRITE_URI_PERMISSION");
    contentResolver.callMethod<void>(
                "releasePersistableUriPermission", "(Landroid/net/Uri;I)V",
                uriObj.object(), flagRead | flagWrite);
#endif
}

bool AndroidUtils::save([[maybe_unused]]const QString& uri,
                        [[maybe_unused]]const QByteArray& array)
{
#ifdef Q_OS_ANDROID
    m_error.clear();
    auto contentResolver = getContentResolver();
    if (! contentResolver.isValid())
        return false;
    auto uriObj = getUriObj(uri);
    if (! uriObj.isValid())
        return false;
    QAndroidJniExceptionCleaner exceptionCleaner;
    auto outputStream = contentResolver.callObjectMethod(
                "openOutputStream",
                "(Landroid/net/Uri;)Ljava/io/OutputStream;", uriObj.object());
    QAndroidJniEnvironment env;
    if (checkException())
        return false;
    if (! outputStream.isValid())
        return false;
    auto byteArray = env->NewByteArray(array.size());
    env->SetByteArrayRegion(byteArray, 0, array.size(),
                            reinterpret_cast<const jbyte*>(array.constData()));
    outputStream.callMethod<void>("write", "([B)V", byteArray);
    if (checkException())
        return false;
    outputStream.callMethod<void>("close", "()V");
    if (env->ExceptionCheck())
        return false;
    return true;
#else
    return false;
#endif
}

bool AndroidUtils::saveImage([[maybe_unused]]const QString& uri,
                             [[maybe_unused]]const QVariant& image)
{
#ifdef Q_OS_ANDROID
    auto img = image.value<QImage>();
    if (img.isNull())
        return false;
    QByteArray array;
    QBuffer buffer(&array);
    buffer.open(QIODevice::WriteOnly);
    img.save(&buffer, "PNG");
    return save(uri, array);
#else
    return false;
#endif
}

//-----------------------------------------------------------------------------
