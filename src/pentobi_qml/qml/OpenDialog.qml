import QtQuick 2.0
import QtQuick.Dialogs 1.2
import "Main.js" as Logic

FileDialog {
    title: qsTr("Open")
    nameFilters: [ qsTr("Blokus games (*.blksgf)"), qsTr("All files (*)") ]
    folder: root.folder == "" ? shortcuts.desktop : root.folder
    onAccepted: {
        root.folder = folder
        gameDisplay.forceActiveFocus() // QTBUG-48456
        lengthyCommand.run(Logic.openFileUrl)
    }
    onRejected: gameDisplay.forceActiveFocus() // QTBUG-48456
}
