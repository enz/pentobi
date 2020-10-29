//-----------------------------------------------------------------------------
/** @file pentobi/qml/SaveDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.FileDialog {
    title: qsTr("Save")
    selectExisting: false
    nameFilterLabels: [ qsTr("Blokus games") ]
    nameFilters: [ [ "*.blksgf" ] ]
    folder: rootWindow.folder
    onAccepted: {
        rootWindow.folder = folder
        Logic.saveFile(Logic.getFileFromUrl(fileUrl))
    }
}
