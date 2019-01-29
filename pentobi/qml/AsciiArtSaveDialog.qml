//-----------------------------------------------------------------------------
/** @file pentobi/qml/AsciiArtSaveDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.FileDialog {
    title: qsTr("Export ASCII Art")
    selectExisting: false
    nameFilterLabels: [ qsTr("Text files") ]
    nameFilters: [ [ "*.txt", "*.TXT" ] ]
    folder: rootWindow.folder
    onAccepted: {
        rootWindow.folder = folder
        Logic.exportAsciiArt(fileUrl)
    }
}
