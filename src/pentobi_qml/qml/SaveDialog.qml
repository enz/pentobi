import QtQuick.Dialogs 1.2
import "Main.js" as Logic

FileDialog {
    title: qsTr("Save")
    selectExisting: false
    folder: root.folder == "" ? shortcuts.desktop : root.folder
    nameFilters: [ qsTr("Blokus games (*.blksgf)"), qsTr("All files (*)") ]
    onAccepted: {
        Logic.saveFileUrl(fileUrl)
        root.folder = folder
        gameDisplay.forceActiveFocus() // QTBUG-48456
    }
    onRejected: gameDisplay.forceActiveFocus() // QTBUG-48456
}
