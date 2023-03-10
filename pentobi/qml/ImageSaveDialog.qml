//-----------------------------------------------------------------------------
/** @file pentobi/qml/ImageSaveDialog.qml
    @author Wing-chung Leung
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Dialogs
import "Main.js" as Logic

FileDialog {
    title: qsTr("Save Image")
    defaultSuffix: "png"
    nameFilters: [ qsTr("PNG image files") + " (*.png)" ]
    currentFolder: rootWindow.folder
    fileMode: FileDialog.SaveFile
    onAccepted: {
        rootWindow.folder = currentFolder
        Logic.exportImage(selectedFile)
    }
}
