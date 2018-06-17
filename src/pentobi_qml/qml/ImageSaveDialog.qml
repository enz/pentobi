import QtQuick 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.FileDialog {
    title: qsTr("Save Image")
    selectExisting: false
    nameFilterText: qsTr("PNG image files (*.png)")
    nameFilter: "*.png"
    folder: rootWindow.folder != "" ? rootWindow.folder : (isAndroid ? "file:///sdcard" : "")
    onAccepted: {
        rootWindow.folder = folder
        Logic.exportImage(fileUrl)
    }
}
