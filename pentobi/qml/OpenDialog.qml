//-----------------------------------------------------------------------------
/** @file pentobi/qml/OpenDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.FileDialog {
    title: qsTr("Open")
    nameFilterLabels: [ qsTr("Blokus games") ]
    nameFilters: [ [ "*.blksgf" ] ]
    folder: rootWindow.folder
    onOpened: name = ""
    onAccepted: {
        rootWindow.folder = folder
        Logic.openFileUrl()
    }
}
