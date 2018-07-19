import QtQuick 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.FileDialog {
    title: isDesktop ? qsTr("Open") : ""
    nameFilterText: qsTr("Blokus games (*.blksgf)")
    nameFilter: "*.blksgf"
    folder: rootWindow.folder
    onOpened: name = ""
    onAccepted: {
        rootWindow.folder = folder
        Logic.openFileUrl()
    }
}
