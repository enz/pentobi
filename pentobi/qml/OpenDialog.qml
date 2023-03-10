//-----------------------------------------------------------------------------
/** @file pentobi/qml/OpenDialog.qml
    @author Wing-chung Leung
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Dialogs
import "Main.js" as Logic

FileDialog {
    title: qsTr("Open")
    nameFilters: [ qsTr("Blokus games") + " (*.blksgf)" ]
    currentFolder: rootWindow.folder
    fileMode: FileDialog.OpenFile
    onAccepted: {
        rootWindow.folder = currentFolder
        if (isAndroid)
            Logic.openFile(selectedFile, "")
        else
            Logic.openFileUrl(selectedFile)
    }
}
