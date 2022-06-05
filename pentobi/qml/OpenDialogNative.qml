//-----------------------------------------------------------------------------
/** @file pentobi/qml/OpenDialogNative.qml
    @author Wing-chung Leung
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
        Logic.openFileUrl(selectedFile)
    }
}
