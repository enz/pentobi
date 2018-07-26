//-----------------------------------------------------------------------------
/** @file pentobi/qml/ImageSaveDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.FileDialog {
    title: isDesktop ? qsTr("Save Image") : ""
    selectExisting: false
    nameFilterLabels: [
        qsTr("PNG image files"),
        qsTr("JPEG image files")
    ]
    nameFilters: [
        [ "*.png", "*.PNG" ],
        [ "*.jpg", "*.JPG", "*.jpeg", "*.JPEG" ]
    ]
    folder: rootWindow.folder
    onAccepted: {
        rootWindow.folder = folder
        Logic.exportImage(fileUrl)
    }
}
