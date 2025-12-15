//-----------------------------------------------------------------------------
/** @file pentobi/RecentFiles.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "RecentFiles.h"

#include <QSettings>
#include "AndroidUtils.h"

using namespace Qt::StringLiterals;

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
        if (entry["file"_L1] == file)
            i.remove();
    }
    QVariantMap entry{ { "file"_L1, file },
                       { "displayName"_L1, displayName } };
    m_entries.prepend(QVariant::fromValue(entry));
    checkMax();
    {
        QSettings settings;
        settings.setValue("recentFiles"_L1, m_entries);
    }
    emit entriesChanged();
}

void RecentFiles::checkMax()
{
    if (m_entries.length() > maxRecentFiles)
        m_entries.resize(maxRecentFiles);
}

void RecentFiles::clear()
{
    m_entries.clear();
    {
        QSettings settings;
        settings.remove("recentFiles"_L1);
    }
    emit entriesChanged();
}

void RecentFiles::load()
{
    {
        QSettings settings;
        m_entries = settings.value("recentFiles"_L1).toList();
    }
    QMutableListIterator i(m_entries);
    while (i.hasNext())
    {
        auto entry = i.next().toMap();
        if (! entry.contains("file"_L1) || ! entry.contains("displayName"_L1))
        {
            i.remove();
            continue;
        }
        auto file = entry["file"_L1].toString();
        if (! AndroidUtils::checkExists(file))
            i.remove();
    }
    checkMax();
    emit entriesChanged();
}

//-----------------------------------------------------------------------------
