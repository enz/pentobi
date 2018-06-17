import QtQuick 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.FileDialog {
    title: qsTr("Open")
    nameFilterText: qsTr("Blokus games (*.blksgf)")
    nameFilter: "*.blksgf"
    folder: rootWindow.folder != "" ? rootWindow.folder : (isAndroid ? "file:///sdcard" : "")
    onAccepted: {
        rootWindow.folder = folder
        Logic.openFileUrl()
    }
}
