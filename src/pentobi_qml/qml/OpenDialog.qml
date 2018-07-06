import QtQuick 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.FileDialog {
    title: isAndroid ? "" : qsTr("Open")
    nameFilterText: qsTr("Blokus games (*.blksgf)")
    nameFilter: "*.blksgf"
    folder: rootWindow.folder
    onAccepted: {
        rootWindow.folder = folder
        Logic.openFileUrl()
    }
}
