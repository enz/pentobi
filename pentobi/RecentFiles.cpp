//-----------------------------------------------------------------------------
/** @file pentobi/RecentFiles.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "RecentFiles.h"

#include <QSettings>
#include "AndroidUtils.h"

using namespace std;

//-----------------------------------------------------------------------------

RecentFiles::RecentFiles(QObject* parent)
    : QObject(parent)
{
    load();
}

RecentFiles::~RecentFiles() = default;

void RecentFiles::add(const QString& file, const QString& displayName)
{
    QMutableListIterator i(m_entries);
    while (i.hasNext())
    {
        auto entry = i.next().value<QVariantMap>();
        if (entry["file"] == file)
            i.remove();
    }
    QVariantMap entry{ { "file", file }, { "displayName", displayName } };
    m_entries.prepend(QVariant::fromValue(entry));
    checkMax(file);
    QSettings settings;
    settings.setValue(QStringLiteral("recentFiles"), m_entries);
    emit entriesChanged();
}

void RecentFiles::checkMax([[maybe_unused]]const QString& currentFile)
{
    while (m_entries.length() > maxRecentFiles)
    {
#ifdef Q_OS_ANDROID
        auto entry = m_entries.last().value<QVariantMap>();
        auto file = entry["file"].toString();
        if (currentFile.isEmpty() || file != currentFile)
            AndroidUtils::releasePersistableUriPermission(file);
#endif
        m_entries.removeLast();
    }
}

void RecentFiles::clear([[maybe_unused]]const QString& currentFile)
{
#ifdef Q_OS_ANDROID
    for (auto& entry : as_const(m_entries))
    {
        auto file = entry.value<QVariantMap>()["file"].toString();
        if (file != currentFile)
            AndroidUtils::releasePersistableUriPermission(file);
    }
#endif
    m_entries.clear();
    QSettings settings;
    settings.setValue(QStringLiteral("recentFiles"), m_entries);
    emit entriesChanged();
}

void RecentFiles::load()
{
    QSettings settings;
    m_entries = settings.value(QStringLiteral("recentFiles")).toList();
    QMutableListIterator i(m_entries);
    while (i.hasNext())
    {
        auto entry = i.next().toMap();
        if (! entry.contains("file") || ! entry.contains("displayName"))
        {
            i.remove();
            continue;
        }
        auto file = entry["file"].toString();
        if (! AndroidUtils::checkExists(file))
            i.remove();
    }
    checkMax();
    emit entriesChanged();
}

//-----------------------------------------------------------------------------
