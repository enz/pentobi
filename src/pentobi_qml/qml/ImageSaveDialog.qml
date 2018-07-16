import QtQuick 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.FileDialog {
    title: isDesktop ? qsTr("Save Image") : ""
    selectExisting: false
    nameFilterText: qsTr("PNG image files (*.png)")
    nameFilter: "*.png"
    folder: rootWindow.folder
    onAccepted: {
        rootWindow.folder = folder
        Logic.exportImage(fileUrl)
    }
}
