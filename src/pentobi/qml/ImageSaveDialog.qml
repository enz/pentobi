//-----------------------------------------------------------------------------
/** @file pentobi/qml/ImageSaveDialog.qml
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtQuick 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.FileDialog {
    title: qsTr("Save Image")
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
    onNameFilterChanged: {
        if (index >= nameFilters.length)
            return
        var pos = name.lastIndexOf(".")
        if (pos < 0)
            return
        var newName = name.substr(0, pos + 1)
        pos = nameFilters[index][0].lastIndexOf(".")
        newName += nameFilters[index][0].substr(pos + 1)
        name = newName
        selectNameField()
    }
    onAccepted: {
        rootWindow.folder = folder
        Logic.exportImage(fileUrl)
    }
}
