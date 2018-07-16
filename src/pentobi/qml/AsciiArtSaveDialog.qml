import QtQuick 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.FileDialog {
    title: isDesktop ? qsTr("Export ASCII Art") : ""
    selectExisting: false
    nameFilterText: qsTr("Text files (*.txt)")
    nameFilter: "*.png"
    folder: rootWindow.folder
    onAccepted: {
        rootWindow.folder = folder
        Logic.exportAsciiArt(fileUrl)
    }
}
