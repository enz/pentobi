//-----------------------------------------------------------------------------
/** @file pentobi/RecentFiles.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_RECENT_FILES_H
#define PENTOBI_RECENT_FILES_H

#include <QObject>

//-----------------------------------------------------------------------------

class RecentFiles
    : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantList entries READ entries NOTIFY entriesChanged)

public:
    static constexpr int maxRecentFiles = 9;


    explicit RecentFiles(QObject* parent = nullptr);

    ~RecentFiles() override;


    Q_INVOKABLE void add(const QString& file, const QString& displayName);

    Q_INVOKABLE void clear(const QString& currentFile);

    const QVariantList& entries() const { return m_entries; }

signals:
    void entriesChanged();

private:
    QVariantList m_entries;


    void checkMax(const QString& currentFile = {});

    void load();
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_RECENT_FILES_H
