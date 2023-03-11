//-----------------------------------------------------------------------------
/** @file pentobi/RecentFiles.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "RecentFiles.h"

#include <QSettings>

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
        if (entry[QStringLiteral("file")] == file)
            i.remove();
    }
    QVariantMap entry{ { "file", file }, { "displayName", displayName } };
    m_entries.prepend(QVariant::fromValue(entry));
    checkMax(file);
    {
        QSettings settings;
        settings.setValue(QStringLiteral("recentFiles"), m_entries);
    }
    emit entriesChanged();
}

void RecentFiles::checkMax([[maybe_unused]]const QString& currentFile)
{
    while (m_entries.length() > maxRecentFiles)
        m_entries.removeLast();
}

void RecentFiles::clear([[maybe_unused]]const QString& currentFile)
{
    m_entries.clear();
    {
        QSettings settings;
        settings.remove(QStringLiteral("recentFiles"));
    }
    emit entriesChanged();
}

void RecentFiles::load()
{
    {
        QSettings settings;
        m_entries = settings.value(QStringLiteral("recentFiles")).toList();
    }
    checkMax();
    emit entriesChanged();
}

//-----------------------------------------------------------------------------
