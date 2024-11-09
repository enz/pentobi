//-----------------------------------------------------------------------------
/** @file pentobi/ImageSaveDialog.qml
    @author Wing-chung Leung
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

import QtCore
import QtQuick.Dialogs
import "main.js" as Logic

FileDialog {
    title: qsTr("Save Image")
    defaultSuffix: "png"
    nameFilters: [ qsTr("PNG image files") + " (*.png)" ]
    currentFolder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)
    fileMode: FileDialog.SaveFile
    onAccepted: Logic.exportImage(selectedFile)
}
