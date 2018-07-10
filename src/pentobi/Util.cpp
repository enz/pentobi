//-----------------------------------------------------------------------------
/** @file pentobi/Util.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Util.h"

#include <QCryptographicHash>
#include <QDialog>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QString>
#include <QUrl>

//-----------------------------------------------------------------------------

namespace Util
{

QString getDataDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::DataLocation);
}

void initDataDir()
{
    QString dataLocation = getDataDir();
    QDir dir(dataLocation);
    if (! dir.exists())
        // Note: dataLocation is an absolute path but there is no static
        // function QDir::mkpath()
        dir.mkpath(dataLocation);
}

void setNoTitle(QDialog& dialog)
{
    // On many platforms, message boxes should have no title but using
    // an emtpy string causes Qt to use the lower-case application name (tested
    // on Linux with Qt 4.8). As a workaround, we set the title to a space
    // character.
    dialog.setWindowTitle(" ");
}

} // namespace Util

//-----------------------------------------------------------------------------
