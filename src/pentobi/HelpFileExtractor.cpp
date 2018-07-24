//-----------------------------------------------------------------------------
/** @file pentobi/HelpFileExtractor.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "HelpFileExtractor.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QStandardPaths>

//-----------------------------------------------------------------------------

HelpFileExtractor::HelpFileExtractor(QObject* parent)
    : QObject(parent)
{
}

QUrl HelpFileExtractor::extract(const QString& language)
{
    QDir tmpDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                + "/pentobi/help/" + language + "/pentobi");
    qDebug() << "m_extractedLanguages.size()" << m_extractedLanguages.size();
    if (! m_extractedLanguages.contains(language))
    {
        m_extractedLanguages.append(language);
        qDebug() << "m_extractedLanguages.size()" << m_extractedLanguages.size();
        QDirIterator it(":qml/help/" + language + "/pentobi");
        while (it.hasNext())
        {
            auto src = it.next();
            QFileInfo fileInfo(src);
            if (! fileInfo.isFile())
                continue;
            auto dest = QFileInfo(tmpDir.absolutePath() + "/"
                                  + fileInfo.fileName());
            dest.dir().mkpath(".");
            QFile::remove(dest.absoluteFilePath());
            QFile::copy(src, dest.absoluteFilePath());
        }
        if (language != "C")
            // Other languages use pictures from C
            extract("C");
    }
    auto file = QFileInfo(tmpDir.absolutePath()
                          + "/index.html").absoluteFilePath();
    return QUrl::fromLocalFile(file);
}

//-----------------------------------------------------------------------------
