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
    erase_if(
        m_entries,
        [&file](const QVariant& value) {
            return value.toMap().value("file"_L1) == file;
        });
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
    erase_if(
        m_entries,
            [](const QVariant& value)
            {
                const auto entry = value.toMap();
                if (! entry.contains("file"_L1)
                    || ! entry.contains("displayName"_L1))
                    return true;
                const auto file = entry.value("file"_L1).toString();
                return ! AndroidUtils::checkExists(file);
            });
    checkMax();
    emit entriesChanged();
}

//-----------------------------------------------------------------------------
