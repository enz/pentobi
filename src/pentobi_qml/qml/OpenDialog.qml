import QtQuick 2.0
import "Main.js" as Logic
import "." as Pentobi

Pentobi.FileDialog {
    title: isAndroid ? "" : qsTr("Open")
    nameFilterText: qsTr("Blokus games (*.blksgf)")
    nameFilter: "*.blksgf"
    folder: rootWindow.folder
    onVisibleChanged: if (visible) name = ""
    onAccepted: {
        rootWindow.folder = folder
        Logic.openFileUrl()
    }
}
