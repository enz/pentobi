//-----------------------------------------------------------------------------
/** @file pentobi/qml/OpenDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick
import "Main.js" as Logic

PentobiFileDialog {
    title: qsTr("Open")
    nameFilterLabels: [ qsTr("Blokus games") ]
    nameFilters: [ [ "*.blksgf" ] ]
    folder: rootWindow.folder
    onAboutToShow: name = ""
    onAccepted: {
        rootWindow.folder = folder
        Logic.openFileUrl(openDialog.item.fileUrl)
    }
}
