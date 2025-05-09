//-----------------------------------------------------------------------------
/** @file pentobi/SaveDialog.qml
    @author Wing-chung Leung
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Dialogs
import "main.js" as Logic

FileDialog {
    title: qsTr("Save")
    defaultSuffix: "blksgf"
    nameFilters: [ qsTr("Blokus games") + " (*.blksgf)" ]
    currentFolder: rootWindow.folder
    fileMode: FileDialog.SaveFile
    onAccepted: {
        rootWindow.folder = currentFolder
        Logic.saveFile(gameModel.urlToFile(selectedFile), "")
    }
}
