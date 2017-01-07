import QtQuick 2.0
import QtQuick.Dialogs 1.2
import "Main.js" as Logic

FileDialog {
    title: qsTr("Save")
    selectExisting: false
    folder: root.folder != "" ? root.folder : (isAndroid ? "file:///sdcard" : shortcuts.desktop)
    nameFilters: [ qsTr("Blokus games (*.blksgf)"), qsTr("All files (*)") ]
    onAccepted: {
        Logic.saveFileUrl(fileUrl)
        root.folder = folder
        gameDisplay.forceActiveFocus() // QTBUG-48456
    }
    onRejected: gameDisplay.forceActiveFocus() // QTBUG-48456
}
