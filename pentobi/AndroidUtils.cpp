//-----------------------------------------------------------------------------
/** @file pentobi/AndroidUtils.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "AndroidUtils.h"

#ifdef Q_OS_ANDROID

#include <QBuffer>
#include <QCoreApplication>
#include <QHash>
#include <QImage>
#include <QVariant>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QAndroidIntent>
#include <QAndroidJniObject>
#include <QAndroidJniExceptionCleaner>
#include <QtAndroid>
#else
#include <QtCore/private/qandroidextras_p.h>
#include <QJniObject>
#endif

#else

#include <QFileInfo>
#include <QStandardPaths>

#endif

using namespace std;

//-----------------------------------------------------------------------------

namespace {

#ifdef Q_OS_ANDROID

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using QJniEnvironment = QAndroidJniEnvironment;
using QJniObject = QAndroidJniObject;
#endif

struct AutoClose
{
    const QJniObject& m_obj;

    ~AutoClose()
    {
        m_obj.callMethod<void>("close");
    }
};

void takePersistableUriPermission(const QJniObject& intent,
                                  const QJniObject& uri);

QJniObject getContext()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return QtAndroid::androidActivity();
#else
    return {QNativeInterface::QAndroidApplication::context()};
#endif
}

QJniObject getContentResolver()
{
    return getContext().callObjectMethod(
                "getContentResolver", "()Landroid/content/ContentResolver;");
}

QString getDisplayName(QJniObject uri)
{
    auto contentResolver = getContentResolver();
    if (! contentResolver.isValid())
        return {};
    QJniEnvironment env;
    auto stringClass = env->FindClass("java/lang/String");
    auto projection = env->NewObjectArray(1, stringClass, nullptr);
    auto column = QJniObject::getStaticObjectField<jstring>(
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
    if (! cursor.callMethod<jboolean>("moveToFirst"))
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

QJniObject getUriObj(const QString& uri)
{
    auto uriString = QJniObject::fromString(uri);
    return QJniObject::callStaticObjectMethod(
                "android/net/Uri", "parse",
                "(Ljava/lang/String;)Landroid/net/Uri;",
                uriString.object<jstring>());
}

void setExtraInitialUri(QJniObject& intent, const QString& uri)
{
    QJniEnvironment env;
    auto extraInitialUriObj =
            QJniObject::getStaticObjectField<jstring>(
                "android/provider/DocumentsContract", "EXTRA_INITIAL_URI");
    // Might throw because EXTRA_INITIAL_URI needs Android 8.0
    if (env->ExceptionCheck())
        return;
    auto value = getUriObj(uri);
    if (! value.isValid())
        return;
    intent.callObjectMethod(
                "putExtra",
                "(Ljava/lang/String;Landroid/os/Parcelable;)Landroid/content/Intent;",
                extraInitialUriObj.object<jstring>(),
                value.object<jstring>());
}

void startActivity(
        const QJniObject& intent, int code,
        const function<void (int, int, const QJniObject &)>& callback)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QtAndroid::startActivity(intent, code, callback);
#else
    QtAndroidPrivate::startActivity(intent, code, callback);
#endif
}

void startDocumentActivity(
        const char* actionField, const QString& type,
        const QString& extraInitialUri, const QString& extraTitle,
        bool takePersistablePermission,
        const function<void(const QString& uri,const QString& displayNamei)>&
        callback)
{
    auto action = QJniObject::getStaticObjectField<jstring>(
                "android/content/Intent", actionField);
    if (! action.isValid())
        return;
    QJniObject intent("android/content/Intent", "(Ljava/lang/String;)V",
                             action.object<jstring>());
    if (! intent.isValid())
        return;
    auto category = QJniObject::getStaticObjectField<jstring>(
                "android/content/Intent", "CATEGORY_OPENABLE");
    if (! category.isValid())
        return;
    intent.callObjectMethod(
                "addCategory",
                "(Ljava/lang/String;)Landroid/content/Intent;",
                category.object<jstring>());
    auto typeObj = QJniObject::fromString(type);
    intent.callObjectMethod(
                "setType",
                "(Ljava/lang/String;)Landroid/content/Intent;",
                typeObj.object<jstring>());
    if (! extraTitle.isEmpty())
    {
        auto extraTitleObj = QJniObject::getStaticObjectField<jstring>(
                    "android/content/Intent", "EXTRA_TITLE");
        if (! extraTitleObj.isValid())
            return;
        auto extraTitleValue = QJniObject::fromString(extraTitle);
        intent.callObjectMethod(
                    "putExtra",
                    "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;",
                    extraTitleObj.object<jstring>(),
                    extraTitleValue.object<jstring>());
    }
    if (! extraInitialUri.isEmpty() && ! QUrl(extraInitialUri).isRelative())
        setExtraInitialUri(intent, extraInitialUri);
    startActivity(intent, 0,
                  [takePersistablePermission, callback](int, int result,
                  const QJniObject& data) {
        auto ok = QJniObject::getStaticField<jint>(
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

void takePersistableUriPermission(const QJniObject& intent,
                                  const QJniObject& uri)
{
    auto contentResolver = getContentResolver();
    if (! contentResolver.isValid())
        return;
    auto flags = intent.callMethod<jint>("getFlags");
    auto flagRead = QJniObject::getStaticField<jint>(
                "android/content/Intent", "FLAG_GRANT_READ_URI_PERMISSION");
    auto flagWrite = QJniObject::getStaticField<jint>(
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
    QJniEnvironment env;
    if (! env->ExceptionCheck())
        return false;
    auto e = env->ExceptionOccurred();
    env->ExceptionClear();
    auto method = env->GetMethodID(env->GetObjectClass(e),
                                   "getLocalizedMessage",
                                   "()Ljava/lang/String;");
    QJniObject message(env->CallObjectMethod(e, method));
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
    QJniEnvironment env;
    auto stringClass = env->FindClass("java/lang/String");
    auto projection = env->NewObjectArray(1, stringClass, nullptr);
    auto column = QJniObject::getStaticObjectField<jstring>(
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
    auto count = cursor.callMethod<jint>("getCount");
    return count > 0;
#else
    return QFileInfo::exists(file);
#endif
}

void AndroidUtils::exit()
{
#ifdef Q_OS_ANDROID
    QJniObject::callStaticMethod<void>(
                "java/lang/System", "exit", "(I)V", 0);
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
    auto intent =
            getContext().callObjectMethod(
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

QStringList AndroidUtils::getPersistedUriPermissions()
{
    QStringList result;
#ifdef Q_OS_ANDROID
    auto contentResolver = getContentResolver();
    if (! contentResolver.isValid())
        return result;
    auto permissions = contentResolver.callObjectMethod(
                "getPersistedUriPermissions", "()Ljava/util/List;");
    if (! permissions.isValid())
        return result;
    auto size = permissions.callMethod<jint>("size");
    for (jint i = 0; i < size; ++i)
    {
        auto p = permissions.callObjectMethod(
                    "get", "(I)Ljava/lang/Object;", i);
        if (! p.isValid())
            continue;
        auto uri = p.callObjectMethod("getUri", "()Landroid/net/Uri;");
        if (! uri.isValid())
            continue;
        auto s = uri.callObjectMethod<jstring>("toString");
        if (! s.isValid())
            continue;
        result.append(s.toString());
    }
#endif
    return result;
}

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
    auto inputStream = contentResolver.callObjectMethod(
                "openInputStream",
                "(Landroid/net/Uri;)Ljava/io/InputStream;", uriObj.object());
    if (checkException())
        return false;
    if (! inputStream.isValid())
        return false;
    sgf.clear();
    QJniEnvironment env;
    const int bufSize = 2048;
    auto byteArray = env->NewByteArray(bufSize);
    jbyte buf[bufSize];
    jint n;
    while ((n = inputStream.callMethod<jint>("read", "([B)I", byteArray)) >= 0)
    {
        if (checkException())
        {
            inputStream.callMethod<void>("close");
            return false;
        }
        env->GetByteArrayRegion(byteArray, 0, n, buf);
        sgf.append(reinterpret_cast<const char*>(&buf[0]), n);
    }
    inputStream.callMethod<void>("close");
    return true;
}
#endif

void AndroidUtils::openImageSaveDialog(
        [[maybe_unused]] const QString& suggestedName)
{
#ifdef Q_OS_ANDROID
    startDocumentActivity("ACTION_CREATE_DOCUMENT", "image/png",
                          "", suggestedName, false,
                          [this](const QString& uri,
                          [[maybe_unused]]const QString& displayName) {
        emit imageSaveDialogAccepted(uri);
    });
#endif
}

void AndroidUtils::openOpenDialog([[maybe_unused]] const QString& suggestedUri)
{
#ifdef Q_OS_ANDROID
    startDocumentActivity("ACTION_OPEN_DOCUMENT", "*/*", suggestedUri, "",
                          true,
                          [this](const QString& uri,
                          const QString& displayName) {
        emit openDialogAccepted(uri, displayName);
    });
#endif
}

void AndroidUtils::openTextSaveDialog()
{
#ifdef Q_OS_ANDROID
    startDocumentActivity("ACTION_CREATE_DOCUMENT", "text/plain", "", "",
                          false,
                          [this](const QString& uri,
                          [[maybe_unused]]const QString& displayName) {
        emit textSaveDialogAccepted(uri);
    });
#endif
}

void AndroidUtils::openSaveDialog(
        [[maybe_unused]] const QString& suggestedUri,
        [[maybe_unused]] const QString& suggestedName)
{
#ifdef Q_OS_ANDROID
    startDocumentActivity("ACTION_CREATE_DOCUMENT", "application/x-blokus-sgf",
                          suggestedUri, suggestedName, true,
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
    if (QUrl(uri).isRelative())
        return;
    auto uriObj = getUriObj(uri);
    if (! uriObj.isValid())
        return;
    auto contentResolver = getContentResolver();
    if (! contentResolver.isValid())
        return;
    auto flagRead = QJniObject::getStaticField<jint>(
                "android/content/Intent", "FLAG_GRANT_READ_URI_PERMISSION");
    auto flagWrite = QJniObject::getStaticField<jint>(
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
    auto outputStream = contentResolver.callObjectMethod(
                "openOutputStream",
                "(Landroid/net/Uri;)Ljava/io/OutputStream;", uriObj.object());
    QJniEnvironment env;
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
    outputStream.callMethod<void>("close");
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
