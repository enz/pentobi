//-----------------------------------------------------------------------------
/** @file pentobi/qml/AsciiArtSaveDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick.Dialogs
import "Main.js" as Logic

FileDialog {
    title: qsTr("Export ASCII Art")
    defaultSuffix: "txt"
    nameFilters: [ qsTr("Text files") + " (*.txt)" ]
    currentFolder: rootWindow.folder
    fileMode: FileDialog.SaveFile
    onAccepted: {
        rootWindow.folder = currentFolder
        Logic.exportAsciiArt(selectedFile)
    }
}
