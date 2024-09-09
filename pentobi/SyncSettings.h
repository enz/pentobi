//-----------------------------------------------------------------------------
/** @file pentobi/SyncSettings.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_SYNC_SETTINGS_H
#define PENTOBI_SYNC_SETTINGS_H

#include <QSettings>
#include <QtQml/qqmlregistration.h>

//-----------------------------------------------------------------------------

/** Settings with sync function.
    This item does not use property bindings like Qt.labs.settings.Settings,
    but get/set methods for getting and setting values at defined times. It
    also provides a sync() method, which is necessary to ensure that pending
    changes are written in case the application in killed on Android soon after
    having been suspended (see QTBUG-70291). Note that Settings.sync()
    introduced in Qt 5.13 does not seem to guarantee that changes are saved
    if the process is killed shortly afterwards (last tested with Qt 5.15.1) */
class SyncSettings
    : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    using QObject::QObject;

    Q_INVOKABLE bool valueBool(const QString& key, bool defaultValue) {
        return m_settings.value(key, defaultValue).toBool(); }

    Q_INVOKABLE int valueInt(const QString& key, int defaultValue) {
        return m_settings.value(key, defaultValue).toInt(); }

    Q_INVOKABLE void setValueBool(const QString& key, bool value) {
        m_settings.setValue(key, value); }

    Q_INVOKABLE void setValueInt(const QString& key, int value) {
        m_settings.setValue(key, value); }

    Q_INVOKABLE void sync() { m_settings.sync(); }

private:
    QSettings m_settings;
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_SYNC_SETTINGS_H
